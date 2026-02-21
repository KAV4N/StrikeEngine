#include "strikepch.h"
#include "PhysicsSystem.h"
#include "StrikeEngine/Scene/Components/PhysicsComponent.h"
#include "StrikeEngine/Scene/Components/RendererComponent.h"
#include "StrikeEngine/Scene/Scene.h"
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/World.h"
#include "StrikeEngine/Asset/Types/Model.h"

#include <btBulletDynamicsCommon.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Strike {

    PhysicsSystem::PhysicsSystem() {
        createPhysicsWorld();
    }

    PhysicsSystem::~PhysicsSystem() {
        cleanupPhysicsWorld();
    }

    void PhysicsSystem::createPhysicsWorld() {
        mCollisionConfiguration = new btDefaultCollisionConfiguration();
        mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
        mBroadphase = new btDbvtBroadphase();
        mSolver = new btSequentialImpulseConstraintSolver();
        mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
        mDynamicsWorld->setGravity(btVector3(mGravity.x, mGravity.y, mGravity.z));
    }

    void PhysicsSystem::cleanupPhysicsWorld() {
        if (!mDynamicsWorld) return;

        for (int i = mDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
            btCollisionObject* obj = mDynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);
            if (body) {
                if (body->getMotionState()) {
                    delete body->getMotionState();
                }
               
                if (body->getUserPointer()) {
                    delete static_cast<entt::entity*>(body->getUserPointer());
                    body->setUserPointer(nullptr);
                }
                if (body->getCollisionShape()) {
                    delete body->getCollisionShape();
                }
            }
            mDynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }

        delete mDynamicsWorld;
        delete mSolver;
        delete mBroadphase;
        delete mDispatcher;
        delete mCollisionConfiguration;

        mDynamicsWorld = nullptr;
        mSolver = nullptr;
        mBroadphase = nullptr;
        mDispatcher = nullptr;
        mCollisionConfiguration = nullptr;
    }

    void PhysicsSystem::setGravity(const glm::vec3& gravity) {
        mGravity = gravity;
        if (mDynamicsWorld) {
            mDynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
        }
    }

    glm::vec3 PhysicsSystem::getGravity() const {
        return mGravity;
    }

    void PhysicsSystem::onUpdate(float dt) {
        Scene* scene = World::get().getScene();
        if (!scene) return;

        auto& registry = scene->getRegistry();
        auto view = registry.view<PhysicsComponent>();

       for (auto entity : view) {
            auto& physics = view.get<PhysicsComponent>(entity);
            Entity ent(entity, scene);
            bool shouldBeActive = ent.isActive() && physics.isActive();

            if (!shouldBeActive) {
                if (physics.mInPhysicsWorld && physics.getRigidBody()) {
                    mDynamicsWorld->removeRigidBody(physics.getRigidBody());
                    physics.mInPhysicsWorld = false;
                }
                continue;
            }

            if (!physics.getRigidBody()) {
                createRigidBody(entity);
                physics.mInPhysicsWorld = true;
            } else if (physics.needsRecreate()) {
                recreatePhysicsBody(entity);
                physics.mInPhysicsWorld = true;
            } else if (!physics.mInPhysicsWorld) {
                mDynamicsWorld->addRigidBody(physics.getRigidBody());
                physics.mInPhysicsWorld = true;
            }

            if (physics.isAnchored()) {
                syncTransformToPhysics(entity, physics, ent);
            }
        }

        mDynamicsWorld->stepSimulation(dt, 10);

        for (auto entity : view) {
            Entity ent(entity, scene);
            auto& physics = view.get<PhysicsComponent>(entity);
            if (!ent.isActive() || !physics.isActive()) continue;

            if (!physics.isAnchored()) {
                syncTransformFromPhysics(entity, physics, ent);
            }
        }
    }

    std::vector<Entity> PhysicsSystem::getCollidingEntities(const Entity& entity) const {
        std::vector<Entity> colliding;

        if (!entity.isValid() || !entity.isActive()) return colliding;

        Scene* scene = entity.getScene();
        auto* physics = scene->getRegistry().try_get<PhysicsComponent>(entity.getHandle());
        if (!physics || !physics->getRigidBody() || !physics->isActive()) return colliding;

        btRigidBody* body = physics->getRigidBody();

        int numManifolds = mDispatcher->getNumManifolds();
        for (int i = 0; i < numManifolds; ++i) {
            btPersistentManifold* manifold = mDispatcher->getManifoldByIndexInternal(i);
            if (manifold->getNumContacts() == 0) continue;

            const btRigidBody* bodyA = static_cast<const btRigidBody*>(manifold->getBody0());
            const btRigidBody* bodyB = static_cast<const btRigidBody*>(manifold->getBody1());

            const btRigidBody* other = (bodyA == body) ? bodyB : (bodyB == body) ? bodyA : nullptr;
            if (other) {
                Entity otherEnt = getEntityFromRigidBody(other);
                auto& collPhys = scene->getRegistry().get<PhysicsComponent>(otherEnt.getHandle()); 
                if (otherEnt.isValid() && otherEnt.isActive() && collPhys.isActive()) {
                    colliding.push_back(otherEnt);
                }
            }
        }

        return colliding;
    }

    bool PhysicsSystem::isColliding(const Entity& entityA, const Entity& entityB) const {
        if (!entityA.isValid() || !entityB.isValid() || entityA == entityB || !mDynamicsWorld) return false;
        if (!entityA.isActive() || !entityB.isActive()) return false;

        Scene* scene = entityA.getScene();
        auto& reg = scene->getRegistry();

        auto* physA = reg.try_get<PhysicsComponent>(entityA.getHandle());
        auto* physB = reg.try_get<PhysicsComponent>(entityB.getHandle());
        if (!physA || !physB || !physA->getRigidBody() || !physB->getRigidBody()) return false;
        if (!physA->isActive() || !physB->isActive()) return false;

        btRigidBody* bodyA = physA->getRigidBody();
        btRigidBody* bodyB = physB->getRigidBody();

        int numManifolds = mDispatcher->getNumManifolds();
        for (int i = 0; i < numManifolds; ++i) {
            btPersistentManifold* manifold = mDispatcher->getManifoldByIndexInternal(i);
            if (manifold->getNumContacts() == 0) continue;

            const btRigidBody* bA = static_cast<const btRigidBody*>(manifold->getBody0());
            const btRigidBody* bB = static_cast<const btRigidBody*>(manifold->getBody1());

            if ((bA == bodyA && bB == bodyB) || (bA == bodyB && bB == bodyA)) {
                return true;
            }
        }
        return false;
    }

    RayHit PhysicsSystem::rayCast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const {
        RayHit hit;
        if (!mDynamicsWorld) return hit;

        glm::vec3 end = origin + glm::normalize(direction) * maxDistance;
        btVector3 from(origin.x, origin.y, origin.z);
        btVector3 to(end.x, end.y, end.z);

        btCollisionWorld::ClosestRayResultCallback callback(from, to);
        mDynamicsWorld->rayTest(from, to, callback);

        if (callback.hasHit()) {
            const btRigidBody* body = btRigidBody::upcast(callback.m_collisionObject);
            if (body) {
                Entity ent = getEntityFromRigidBody(body);
                Scene* scene = ent.getScene();
                auto& collPhys = scene->getRegistry().get<PhysicsComponent>(ent.getHandle()); 

                if (ent.isValid() && ent.isActive() && collPhys.isActive()) {
                    hit.entity = ent;
                    hit.distance = callback.m_closestHitFraction * maxDistance;
                    hit.normal = glm::vec3(callback.m_hitNormalWorld.x(), callback.m_hitNormalWorld.y(), callback.m_hitNormalWorld.z());
                }
            }
        }
        return hit;
    }

    std::vector<RayHit> PhysicsSystem::rayCastAll(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const {
        std::vector<RayHit> hits;
        if (!mDynamicsWorld) return hits;

        glm::vec3 end = origin + glm::normalize(direction) * maxDistance;
        btVector3 from(origin.x, origin.y, origin.z);
        btVector3 to(end.x, end.y, end.z);

        btCollisionWorld::AllHitsRayResultCallback callback(from, to);
        mDynamicsWorld->rayTest(from, to, callback);

        if (callback.hasHit()) {
            for (int i = 0; i < callback.m_hitFractions.size(); ++i) {
                const btRigidBody* body = btRigidBody::upcast(callback.m_collisionObjects[i]);
                if (body) {
                    Entity ent = getEntityFromRigidBody(body);

                    Scene* scene = ent.getScene();
                    auto& collPhys = scene->getRegistry().get<PhysicsComponent>(ent.getHandle()); 

                    if (ent.isValid() && ent.isActive() && collPhys.isActive()) {
                        RayHit hit;
                        hit.entity = ent;
                        hit.distance = callback.m_hitFractions[i] * maxDistance;
                        hit.normal = glm::vec3(callback.m_hitNormalWorld[i].x(), callback.m_hitNormalWorld[i].y(), callback.m_hitNormalWorld[i].z());
                        hits.push_back(hit);
                    }
                }
            }
            std::sort(hits.begin(), hits.end(), [](const RayHit& a, const RayHit& b) { return a.distance < b.distance; });
        }
        return hits;
    }

    Entity PhysicsSystem::getEntityFromRigidBody(const btRigidBody* body) const {
        if (!body || !body->getUserPointer()) return Entity();

        entt::entity handle = *static_cast<entt::entity*>(body->getUserPointer());
        Scene* scene = World::get().getScene();
        if (scene && scene->getRegistry().valid(handle)) {
            return Entity(handle, scene);
        }
        return Entity();
    }

    void PhysicsSystem::createRigidBody(entt::entity entityHandle) {
        Scene* scene = World::get().getScene();
        if (!scene) return;

        auto& registry = scene->getRegistry();
        Entity ent(entityHandle, scene);
        auto& physics = registry.get<PhysicsComponent>(entityHandle);

        const glm::mat4& worldMat = ent.getWorldMatrix();
        glm::vec3 pos, scale, skew;
        glm::quat rot;
        glm::vec4 persp;
        glm::decompose(worldMat, scale, rot, pos, skew, persp);

        glm::vec3 size(1.0f);
        glm::vec3 centerOffset(0.0f);

        auto* renderer = registry.try_get<RendererComponent>(entityHandle);

        if (renderer && renderer->isActive()) {
            auto model = renderer->getModel();
            auto mesh = renderer->getMesh();

            if (model)
            {
                Bounds bounds;
                if (mesh) 
                    bounds = mesh->getBounds();
                else 
                    bounds = model->getBounds();
                size = bounds.aabbMax - bounds.aabbMin;
                centerOffset = (bounds.aabbMax + bounds.aabbMin) * 0.5f;
            }
           
            
        }

        physics.setSize(size);
        physics.setCenter(centerOffset);

        glm::vec3 half = size * scale * 0.5f;
        btBoxShape* shape = new btBoxShape(btVector3(half.x, half.y, half.z));
        physics.setCollisionShape(shape);

        btScalar mass = physics.isAnchored() ? 0.0f : physics.getMass();
        btVector3 inertia(0, 0, 0);
        if (mass > 0.0f) shape->calculateLocalInertia(mass, inertia);

        glm::vec3 offsetRotated = rot * (centerOffset * scale);
        glm::vec3 adjustedPos = pos + offsetRotated;

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(adjustedPos.x, adjustedPos.y, adjustedPos.z));
        startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

        btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

        btRigidBody::btRigidBodyConstructionInfo info(mass, motionState, shape, inertia);
        info.m_friction = physics.getFriction();
        info.m_restitution = physics.getBounciness();
        info.m_linearDamping = physics.getLinearDamping();
        info.m_angularDamping = physics.getAngularDamping();

        btRigidBody* body = new btRigidBody(info);

        if (physics.isAnchored()) {
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        }
        body->setActivationState(DISABLE_DEACTIVATION);

        if (!physics.canCollide()) {
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }

        body->setUserPointer(new entt::entity(entityHandle));

        physics.setRigidBody(body);
        mDynamicsWorld->addRigidBody(body);
        applyPendingValues(physics);
        physics.clearRecreate();
    }

    void PhysicsSystem::removePhysics(entt::entity entityHandle) {
        Scene* scene = World::get().getScene();
        if (!scene) return;

        auto& registry = scene->getRegistry();
        auto* physics = registry.try_get<PhysicsComponent>(entityHandle);
        if (!physics) return;

        btRigidBody* body = physics->getRigidBody();
        if (body) {
            mDynamicsWorld->removeRigidBody(body);
            physics->mInPhysicsWorld = false;
            if (auto collShape = body->getCollisionShape()) delete collShape;
            if (body->getMotionState()) delete body->getMotionState();
            if (body->getUserPointer()) delete static_cast<entt::entity*>(body->getUserPointer());
            delete body;
            physics->setRigidBody(nullptr);
            physics->setCollisionShape(nullptr);
        }
    }

    void PhysicsSystem::recreatePhysicsBody(entt::entity entityHandle) {
        Scene* scene = World::get().getScene();
        if (!scene) return;

        auto& registry = scene->getRegistry();
        auto& physics = registry.get<PhysicsComponent>(entityHandle);

        glm::vec3 vel(0.0f), angVel(0.0f);
        bool preserve = false;
        if (physics.getRigidBody() && !physics.isAnchored()) {
            vel = physics.getVelocity();
            angVel = physics.getAngularVelocity();
            preserve = true;
        }

        removePhysics(entityHandle);
        createRigidBody(entityHandle);
        
        mDynamicsWorld->addRigidBody(physics.getRigidBody());

        if (preserve && !physics.isAnchored()) {
            physics.setVelocity(vel);
            physics.setAngularVelocity(angVel);
        }

    }

    void PhysicsSystem::applyPendingValues(PhysicsComponent& physics) {
        if (!physics.getRigidBody() || physics.isAnchored()) return;

        if (physics.mPendingVelocity != glm::vec3(0.0f)) physics.setVelocity(physics.mPendingVelocity);
        if (physics.mPendingAngularVelocity != glm::vec3(0.0f)) physics.setAngularVelocity(physics.mPendingAngularVelocity);
        if (physics.mPendingImpulse != glm::vec3(0.0f)) {
            physics.applyImpulse(physics.mPendingImpulse);
            physics.mPendingImpulse = glm::vec3(0.0f);
        }
    }

    void PhysicsSystem::syncTransformFromPhysics(entt::entity handle, PhysicsComponent& physics, Entity& ent) {
        btRigidBody* body = physics.getRigidBody();
        if (!body) return;

        btTransform trans;
        if (body->getMotionState()) {
            body->getMotionState()->getWorldTransform(trans);
        } else {
            trans = body->getWorldTransform();
        }

        glm::vec3 pos(trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z());
        glm::quat rot(trans.getRotation().w(), trans.getRotation().x(), trans.getRotation().y(), trans.getRotation().z());

        glm::vec3 offset = physics.getCenter() * ent.getScale();
        glm::vec3 correctedPos = pos - rot * offset;

        glm::mat4 world = glm::translate(glm::mat4(1.0f), correctedPos) * glm::mat4_cast(rot) * glm::scale(glm::mat4(1.0f), ent.getScale());
        glm::mat4 parentWorld = ent.getParent().isValid() ? ent.getParent().getWorldMatrix() : glm::mat4(1.0f);
        glm::mat4 local = glm::inverse(parentWorld) * world;

        glm::vec3 localPos, localScale, skew;
        glm::quat localRot;
        glm::vec4 persp;
        glm::decompose(local, localScale, localRot, localPos, skew, persp);

        ent.setPosition(localPos);
        ent.setRotation(localRot);
    }

    void PhysicsSystem::syncTransformToPhysics(entt::entity handle, PhysicsComponent& physics, Entity& ent) {
        btRigidBody* body = physics.getRigidBody();
        if (!body) return;

        glm::vec3 pos = ent.getWorldPosition();
        glm::quat rot = ent.getWorldRotation();
        glm::vec3 scale = ent.getWorldScale();

        glm::vec3 offset = physics.getCenter() * scale;
        glm::vec3 adjustedPos = pos + rot * offset;

        btTransform trans;
        trans.setOrigin(btVector3(adjustedPos.x, adjustedPos.y, adjustedPos.z));
        trans.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

        body->setWorldTransform(trans);
        if (body->getMotionState()) body->getMotionState()->setWorldTransform(trans);

        body->setLinearVelocity(btVector3(0,0,0));
        body->setAngularVelocity(btVector3(0,0,0));
        body->clearForces();
    }

} 
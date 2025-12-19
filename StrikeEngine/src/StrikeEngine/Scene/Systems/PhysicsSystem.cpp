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

namespace StrikeEngine {

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

        for (int i = mDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
            btCollisionObject* obj = mDynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);
            if (body && body->getMotionState()) {
                delete body->getMotionState();
            }
            mDynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }

        for (auto& pair : mCollisionShapes) {
            delete pair.second;
        }
        mCollisionShapes.clear();

        delete mDynamicsWorld;
        delete mSolver;
        delete mBroadphase;
        delete mDispatcher;
        delete mCollisionConfiguration;

        mDynamicsWorld = nullptr;
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
        if (!mDynamicsWorld || !scene) return;

        auto& registry = scene->getRegistry();
        auto view = registry.view<PhysicsComponent>();

        // Initialize or update physics bodies
        for (auto entity : view) {
            auto& physics = view.get<PhysicsComponent>(entity);

            if (!physics.getRigidBody()) {
                createRigidBody(entity);
            }
            else if (physics.needsRecreate()) {
                recreatePhysicsBody(entity);
            }

            if (physics.isAnchored()) {
                Entity ent(entity, scene);
                syncTransformToPhysics(entity, physics, ent);
            }
        }

        mDynamicsWorld->stepSimulation(dt, 10);

        // Sync physics results back to transforms
        for (auto entity : view) {
            auto& physics = view.get<PhysicsComponent>(entity);
            
            if (!physics.isAnchored()) {
                Entity ent(entity, scene);
                syncTransformFromPhysics(entity, physics, ent);
            }
        }
    }

    std::vector<Entity> PhysicsSystem::getCollidingEntities(const Entity& entity) const {
        std::vector<Entity> collidingEntities;

        if (!entity.isValid()) return collidingEntities;

        Scene* scene = entity.getScene();
        if (!scene || !mDynamicsWorld) return collidingEntities;

        entt::entity handle = entity.getHandle();
        auto& registry = scene->getRegistry();
        auto* physics = registry.try_get<PhysicsComponent>(handle);
        if (!physics) return collidingEntities;

        btRigidBody* body = physics->getRigidBody();
        if (!body) return collidingEntities;

        // Iterate through all contact manifolds in the dispatcher
        int numManifolds = mDispatcher->getNumManifolds();
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold* contactManifold = mDispatcher->getManifoldByIndexInternal(i);
            
            // Check if there are any actual contact points
            int numContacts = contactManifold->getNumContacts();
            if (numContacts == 0) continue;

            const btCollisionObject* objA = contactManifold->getBody0();
            const btCollisionObject* objB = contactManifold->getBody1();

            const btRigidBody* bodyA = btRigidBody::upcast(objA);
            const btRigidBody* bodyB = btRigidBody::upcast(objB);

            // Check if our body is involved in this collision
            const btRigidBody* otherBody = nullptr;
            if (bodyA == body) {
                otherBody = bodyB;
            } else if (bodyB == body) {
                otherBody = bodyA;
            } else {
                continue; // This collision doesn't involve our entity
            }

            if (!otherBody) continue;

            // Find the entity that has this rigid body in its PhysicsComponent
            auto view = registry.view<PhysicsComponent>();
            for (auto otherEntity : view) {
                auto& otherPhysics = view.get<PhysicsComponent>(otherEntity);
                if (otherPhysics.getRigidBody() == otherBody) {
                    collidingEntities.emplace_back(otherEntity, scene);
                    break;
                }
            }
        }

        return collidingEntities;
    }

    bool PhysicsSystem::isColliding(const Entity& entityA, const Entity& entityB) const {
        if (!entityA.isValid() || !entityB.isValid()) return false;
        if (!mDynamicsWorld) return false;
        if (entityA == entityB) return false;

        Scene* scene = entityA.getScene();

        auto& registry = scene->getRegistry();
        
        entt::entity handleA = entityA.getHandle();
        entt::entity handleB = entityB.getHandle();

        auto* physicsA = registry.try_get<PhysicsComponent>(handleA);
        auto* physicsB = registry.try_get<PhysicsComponent>(handleB);
        
        if (!physicsA || !physicsB) return false;

        btRigidBody* bodyA = physicsA->getRigidBody();
        btRigidBody* bodyB = physicsB->getRigidBody();
        
        if (!bodyA || !bodyB) return false;

        // Iterate through all contact manifolds in the dispatcher
        int numManifolds = mDispatcher->getNumManifolds();
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold* contactManifold = mDispatcher->getManifoldByIndexInternal(i);
            
            // Check if there are any actual contact points
            int numContacts = contactManifold->getNumContacts();
            if (numContacts == 0) continue;

            const btCollisionObject* objA = contactManifold->getBody0();
            const btCollisionObject* objB = contactManifold->getBody1();

            const btRigidBody* manifoldBodyA = btRigidBody::upcast(objA);
            const btRigidBody* manifoldBodyB = btRigidBody::upcast(objB);

            // Check if both bodies are involved in this collision
            if ((manifoldBodyA == bodyA && manifoldBodyB == bodyB) ||
                (manifoldBodyA == bodyB && manifoldBodyB == bodyA)) {
                return true;
            }
        }

        return false;
    }

    void PhysicsSystem::removePhysics(entt::entity entity) {
        Scene* scene = World::get().getScene();
        if (!scene) return;

        auto& registry = scene->getRegistry();
        auto* physics = registry.try_get<PhysicsComponent>(entity);
        if (!physics) return;

        btRigidBody* body = physics->getRigidBody();
        if (body) {
            mDynamicsWorld->removeRigidBody(body);
            if (body->getMotionState()) delete body->getMotionState();
            delete body;
            physics->setRigidBody(nullptr);
        }

        auto it = mCollisionShapes.find(entity);
        if (it != mCollisionShapes.end()) {
            delete it->second;
            mCollisionShapes.erase(it);
        }

        physics->setCollisionShape(nullptr);
    }

    void PhysicsSystem::createRigidBody(entt::entity entity) {
        Scene* scene = World::get().getScene();
        if (!scene) return;

        auto& registry = scene->getRegistry();
        Entity ent(entity, scene);
        auto& physics = registry.get<PhysicsComponent>(entity);
        
        // Use Entity's world matrix
        const glm::mat4& worldMatrix = ent.getWorldMatrix();

        glm::vec3 worldPosition, worldScale, skew;
        glm::quat worldRotation;
        glm::vec4 perspective;
        glm::decompose(worldMatrix, worldScale, worldRotation, worldPosition, skew, perspective);

        glm::vec3 localSize(1.0f);
        glm::vec3 meshMidPoint(0.0f);

        auto* renderer = registry.try_get<RendererComponent>(entity);
        if (renderer && renderer->hasModel()) {
            auto model = renderer->getModel();
            Bounds bounds;

            if (renderer->hasMesh()) {
                auto mesh = renderer->getMesh();
                bounds = mesh ? mesh->getBounds() : model->getBounds();
            } else {
                bounds = model->getBounds();
            }

            localSize = bounds.aabbMax - bounds.aabbMin;
            meshMidPoint = (bounds.aabbMax + bounds.aabbMin) * 0.5f;

            physics.setSize(localSize);
            physics.setCenter(glm::vec3(0.0f));
            physics.mBulletCenterOffset = meshMidPoint;
        }

        glm::vec3 scaledSize = localSize * worldScale;
        glm::vec3 rotatedOffset = worldRotation * (meshMidPoint * worldScale);

        btVector3 halfExtents(scaledSize.x * 0.5f, scaledSize.y * 0.5f, scaledSize.z * 0.5f);
        btBoxShape* boxShape = new btBoxShape(halfExtents);

        physics.setCollisionShape(boxShape);
        mCollisionShapes[entity] = boxShape;

        btScalar mass = physics.isAnchored() ? 0.0f : physics.getMass();
        bool isDynamic = (mass > 0.0f);

        btVector3 localInertia(0, 0, 0);
        if (isDynamic) boxShape->calculateLocalInertia(mass, localInertia);

        glm::vec3 adjustedPosition = worldPosition + rotatedOffset;

        btTransform trans;
        trans.setIdentity();
        trans.setOrigin(btVector3(adjustedPosition.x, adjustedPosition.y, adjustedPosition.z));
        trans.setRotation(btQuaternion(worldRotation.x, worldRotation.y, worldRotation.z, worldRotation.w));

        btDefaultMotionState* motionState = new btDefaultMotionState(trans);

        btRigidBody::btRigidBodyConstructionInfo info(mass, motionState, boxShape, localInertia);
        info.m_friction = physics.getFriction();
        info.m_restitution = physics.getBounciness();
        info.m_linearDamping = physics.getLinearDamping();
        info.m_angularDamping = physics.getAngularDamping();

        btRigidBody* body = new btRigidBody(info);

        if (physics.isAnchored()) {
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            
        }

        if (!physics.canCollide()) {
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }

        body->setActivationState(DISABLE_DEACTIVATION);

        physics.setRigidBody(body);
        mDynamicsWorld->addRigidBody(body);

        // Apply any pending values
        applyPendingValues(physics);

        physics.clearRecreate();
    }

    void PhysicsSystem::recreatePhysicsBody(entt::entity entity) {
        Scene* scene = World::get().getScene();
        if (!scene) return;

        auto& registry = scene->getRegistry();
        auto& physics = registry.get<PhysicsComponent>(entity);

        // Store current velocities if not anchored
        glm::vec3 currentVelocity(0.0f);
        glm::vec3 currentAngularVelocity(0.0f);
        bool shouldPreserveVelocity = false;

        if (physics.getRigidBody() && !physics.isAnchored()) {
            // Only preserve velocities if we're not changing anchored state
            currentVelocity = physics.getVelocity();
            currentAngularVelocity = physics.getAngularVelocity();
            shouldPreserveVelocity = true;
        }

        // Remove old physics body
        removePhysics(entity);

        // Create new physics body
        createRigidBody(entity);

        // Restore velocities if appropriate (and not changing anchored state)
        if (shouldPreserveVelocity && !physics.isAnchored()) {
            physics.setVelocity(currentVelocity);
            physics.setAngularVelocity(currentAngularVelocity);
        }
    }

    void PhysicsSystem::applyPendingValues(PhysicsComponent& physics) {
        if (!physics.getRigidBody() || physics.isAnchored()) return;

        // Apply pending velocity if it's not zero
        if (physics.mPendingVelocity != glm::vec3(0.0f)) {
            physics.setVelocity(physics.mPendingVelocity);
        }

        // Apply pending angular velocity if it's not zero
        if (physics.mPendingAngularVelocity != glm::vec3(0.0f)) {
            physics.setAngularVelocity(physics.mPendingAngularVelocity);
        }

        // Apply pending impulse if it's not zero
        if (physics.mPendingImpulse != glm::vec3(0.0f)) {
            physics.applyImpulse(physics.mPendingImpulse);
            physics.mPendingImpulse = glm::vec3(0.0f);
        }
    }

    void PhysicsSystem::syncTransformFromPhysics(entt::entity entity, PhysicsComponent& physics, Entity& ent) {
        btRigidBody* body = physics.getRigidBody();
        if (!body) return;

        btTransform trans;
        if (body->getMotionState()) {
            body->getMotionState()->getWorldTransform(trans);
        } else {
            trans = body->getWorldTransform();
        }

        btVector3 origin = trans.getOrigin();
        btQuaternion rot = trans.getRotation();

        glm::vec3 physPos(origin.x(), origin.y(), origin.z());
        glm::quat physRot(rot.w(), rot.x(), rot.y(), rot.z());

        // Remove mesh center offset to get actual world position
        glm::vec3 offset = physics.getBulletCenterOffset();
        glm::vec3 scale = ent.getScale();
        glm::vec3 rotatedOffset = physRot * (offset * scale);
        glm::vec3 worldPos = physPos - rotatedOffset;

        // Build world transform matrix
        glm::mat4 worldMatrix = glm::translate(glm::mat4(1.0f), worldPos) * 
                                glm::mat4_cast(physRot) * 
                                glm::scale(glm::mat4(1.0f), scale);

        // Get parent's world matrix
        Entity parent = ent.getParent();
        glm::mat4 parentWorld = parent.isValid() ? parent.getWorldMatrix() : glm::mat4(1.0f);

        // Convert world transform to local space
        glm::mat4 localMatrix = glm::inverse(parentWorld) * worldMatrix;

        // Decompose local matrix to get local transform components
        glm::vec3 localPos, localScale, skew;
        glm::quat localRot;
        glm::vec4 perspective;
        glm::decompose(localMatrix, localScale, localRot, localPos, skew, perspective);

        // Update local transform via Entity API
        ent.setPosition(localPos);
        ent.setRotation(localRot);
    }

    void PhysicsSystem::syncTransformToPhysics(entt::entity entity, PhysicsComponent& physics, Entity& ent) {
        btRigidBody* body = physics.getRigidBody();
        if (!body) return;

        const glm::mat4& worldMatrix = ent.getWorldMatrix();

        glm::vec3 worldPosition, worldScale, skew;
        glm::quat worldRotation;
        glm::vec4 perspective;
        glm::decompose(worldMatrix, worldScale, worldRotation, worldPosition, skew, perspective);

        glm::vec3 offset = physics.getBulletCenterOffset();
        glm::vec3 rotatedOffset = worldRotation * (offset * worldScale);
        glm::vec3 adjustedPos = worldPosition + rotatedOffset;

        btTransform trans;
        trans.setIdentity();
        trans.setOrigin(btVector3(adjustedPos.x, adjustedPos.y, adjustedPos.z));
        trans.setRotation(btQuaternion(worldRotation.x, worldRotation.y, worldRotation.z, worldRotation.w));

        body->setWorldTransform(trans);
        if (body->getMotionState()) body->getMotionState()->setWorldTransform(trans);

        // Clear velocities only for anchored objects
        body->setLinearVelocity(btVector3(0,0,0));
        body->setAngularVelocity(btVector3(0,0,0));
    }

} // namespace StrikeEngine
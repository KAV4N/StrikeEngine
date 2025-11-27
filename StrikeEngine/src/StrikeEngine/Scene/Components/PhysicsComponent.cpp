#include "PhysicsComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"
#include <btBulletDynamicsCommon.h>

namespace StrikeEngine {
    REGISTER_COMPONENT(PhysicsComponent)

    PhysicsComponent::~PhysicsComponent() = default;

    void PhysicsComponent::deserialize(const pugi::xml_node& node) {
        mAnchored = node.attribute("anchored").as_bool(false);
        mCanCollide = node.attribute("canCollide").as_bool(true);
        mMass = node.attribute("mass").as_float(1.0f);
        mFriction = node.attribute("friction").as_float(0.5f);
        mRestitution = node.attribute("restitution").as_float(0.0f);
        mLinearDamping = node.attribute("linearDamping").as_float(0.0f);
        mAngularDamping = node.attribute("angularDamping").as_float(0.05f);

        if (auto sizeNode = node.child("collisionSize")) {
            mSize.x = sizeNode.attribute("x").as_float(1.0f);
            mSize.y = sizeNode.attribute("y").as_float(1.0f);
            mSize.z = sizeNode.attribute("z").as_float(1.0f);
        }

        mNeedsRecreate = true;
    }

    void PhysicsComponent::serialize(pugi::xml_node& node) const {
        node.append_attribute("anchored") = mAnchored;
        node.append_attribute("canCollide") = mCanCollide;
        node.append_attribute("mass") = mMass;
        node.append_attribute("friction") = mFriction;
        node.append_attribute("restitution") = mRestitution;
        node.append_attribute("linearDamping") = mLinearDamping;
        node.append_attribute("angularDamping") = mAngularDamping;

        auto sizeNode = node.append_child("collisionSize");
        sizeNode.append_attribute("x") = mSize.x;
        sizeNode.append_attribute("y") = mSize.y;
        sizeNode.append_attribute("z") = mSize.z;
    }

    void PhysicsComponent::setAnchored(bool anchored) {
        if (mAnchored != anchored) {
            mAnchored = anchored;
            // Changing anchored state requires recreating the body and clearing forces
            mNeedsRecreate = true;
        }
    }

    void PhysicsComponent::setCanCollide(bool canCollide) {
        mCanCollide = canCollide;
        if (mRigidBody) {
            if (canCollide) {
                mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
            } else {
                mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
            }
        }
    }

    void PhysicsComponent::setMass(float mass) {
        mMass = mass;
        if (mRigidBody && !mAnchored) {
            btVector3 localInertia(0, 0, 0);
            if (mass > 0.0f && mCollisionShape) {
                mCollisionShape->calculateLocalInertia(mass, localInertia);
            }
            mRigidBody->setMassProps(mass, localInertia);
            mRigidBody->updateInertiaTensor();
        }
    }

    void PhysicsComponent::setSize(const glm::vec3& size) {
        if (mSize != size) {
            mSize = size;
            // Size change requires recreating the collision shape
            mNeedsRecreate = true;
        }
    }

    void PhysicsComponent::setCenter(const glm::vec3& center) {
        if (mCenter != center) {
            mCenter = center;
            // Center change requires recreating the body
            mNeedsRecreate = true;
        }
    }

    // Linear velocity
    void PhysicsComponent::setVelocity(const glm::vec3& velocity) {
        mPendingVelocity = velocity;
        if (mRigidBody && !mAnchored) {
            mRigidBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
            mRigidBody->activate();
        }
    }

    glm::vec3 PhysicsComponent::getVelocity() const {
        if (mRigidBody) {
            const btVector3& v = mRigidBody->getLinearVelocity();
            return glm::vec3(v.x(), v.y(), v.z());
        }
        return mPendingVelocity;
    }

    // Angular velocity
    void PhysicsComponent::setAngularVelocity(const glm::vec3& angularVel) {
        mPendingAngularVelocity = angularVel;
        if (mRigidBody && !mAnchored) {
            mRigidBody->setAngularVelocity(btVector3(angularVel.x, angularVel.y, angularVel.z));
            mRigidBody->activate();
        }
    }

    glm::vec3 PhysicsComponent::getAngularVelocity() const {
        if (mRigidBody) {
            const btVector3& av = mRigidBody->getAngularVelocity();
            return glm::vec3(av.x(), av.y(), av.z());
        }
        return mPendingAngularVelocity;
    }

    void PhysicsComponent::applyImpulse(const glm::vec3& impulse) {
        mPendingImpulse += impulse;
        if (mRigidBody && !mAnchored) {
            mRigidBody->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
            mRigidBody->activate();
        }
    }

    void PhysicsComponent::setFriction(float friction) {
        mFriction = friction;
        if (mRigidBody) {
            mRigidBody->setFriction(friction);
        }
    }

    void PhysicsComponent::setBounciness(float restitution) {
        mRestitution = restitution;
        if (mRigidBody) {
            mRigidBody->setRestitution(restitution);
        }
    }

    void PhysicsComponent::setLinearDamping(float damping) {
        mLinearDamping = damping;
        if (mRigidBody) {
            mRigidBody->setDamping(mLinearDamping, mAngularDamping);
        }
    }

    void PhysicsComponent::setAngularDamping(float damping) {
        mAngularDamping = damping;
        if (mRigidBody) {
            mRigidBody->setDamping(mLinearDamping, mAngularDamping);
        }
    }
}
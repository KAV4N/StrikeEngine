#include "strikepch.h"
#include "PhysicsComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"
#include <btBulletDynamicsCommon.h>

namespace StrikeEngine {
    REGISTER_COMPONENT(PhysicsComponent)

    void PhysicsComponent::deserialize(const pugi::xml_node& node) {
        mAnchored = node.attribute("anchored").as_bool(false);
        mCanCollide = node.attribute("collide").as_bool(true);
        mMass = node.attribute("mass").as_float(1.0f);
        mFriction = node.attribute("friction").as_float(0.5f);
        mRestitution = node.attribute("restitution").as_float(0.0f);
        mLinearDamping = node.attribute("lDamping").as_float(0.0f);
        mAngularDamping = node.attribute("aDamping").as_float(0.05f);

        if (auto sizeNode = node.child("size")) {
            mSize.x = sizeNode.attribute("x").as_float(1.0f);
            mSize.y = sizeNode.attribute("y").as_float(1.0f);
            mSize.z = sizeNode.attribute("z").as_float(1.0f);
        }

        mNeedsRecreate = true;

        
        if (auto attr = node.attribute("active")) {
            setActive(attr.as_bool(true));
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
            btVector3 inertia(0,0,0);
            if (mass > 0.0f && mCollisionShape) {
                mCollisionShape->calculateLocalInertia(mass, inertia);
            }
            mRigidBody->setMassProps(mass, inertia);
            mRigidBody->updateInertiaTensor();
        }
    }

    void PhysicsComponent::setSize(const glm::vec3& size) {
        if (mSize != size) {
            mSize = size;
            mNeedsRecreate = true;
        }
    }

    void PhysicsComponent::setCenter(const glm::vec3& center) {
        if (mCenter != center) {
            mCenter = center;
            mNeedsRecreate = true;
        }
    }

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
        if (mRigidBody) mRigidBody->setFriction(friction);
    }

    void PhysicsComponent::setBounciness(float restitution) {
        mRestitution = restitution;
        if (mRigidBody) mRigidBody->setRestitution(restitution);
    }

    void PhysicsComponent::setLinearDamping(float damping) {
        mLinearDamping = damping;
        if (mRigidBody) mRigidBody->setDamping(mLinearDamping, mAngularDamping);
    }

    void PhysicsComponent::setAngularDamping(float damping) {
        mAngularDamping = damping;
        if (mRigidBody) mRigidBody->setDamping(mLinearDamping, mAngularDamping);
    }
}
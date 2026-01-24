#pragma once
#include "Component.h"
#include "StrikeEngine/Scene/Entity.h"

#include <glm/glm.hpp>
#include <pugixml.hpp>


class btRigidBody;
class btCollisionShape;

namespace StrikeEngine {

    struct RayHit {
        Entity entity;
        glm::vec3 normal = glm::vec3(0.0f);
        float distance = 0.0f;

        RayHit() = default;
        bool hasHit() const { return entity.isValid(); }
    };

    class PhysicsComponent : public Component {
    public:
        PhysicsComponent() = default;
        ~PhysicsComponent() = default;

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "physics";
            return typeName;
        }

        const std::string& getTypeName() const override { return getStaticTypeName(); }

        void deserialize(const pugi::xml_node& node) override;

        void setAnchored(bool anchored);
        bool isAnchored() const { return mAnchored; }

        void setCanCollide(bool canCollide);
        bool canCollide() const { return mCanCollide; }

        void setMass(float mass);
        float getMass() const { return mMass; }

        void setSize(const glm::vec3& size);
        const glm::vec3& getSize() const { return mSize; }

        void setCenter(const glm::vec3& center);
        const glm::vec3& getCenter() const { return mCenter; }

        void setVelocity(const glm::vec3& velocity);
        glm::vec3 getVelocity() const;

        void setAngularVelocity(const glm::vec3& angularVel);
        glm::vec3 getAngularVelocity() const;

        void applyImpulse(const glm::vec3& impulse);

        void setFriction(float friction);
        float getFriction() const { return mFriction; }

        void setBounciness(float restitution);
        float getBounciness() const { return mRestitution; }

        void setLinearDamping(float damping);
        float getLinearDamping() const { return mLinearDamping; }

        void setAngularDamping(float damping);
        float getAngularDamping() const { return mAngularDamping; }

        bool needsRecreate() const { return mNeedsRecreate; }
        void clearRecreate() { mNeedsRecreate = false; }

    private:
        friend class PhysicsSystem;

        btRigidBody* getRigidBody() const { return mRigidBody; }
        void setRigidBody(btRigidBody* body) { mRigidBody = body; }

        btCollisionShape* getCollisionShape() const { return mCollisionShape; }
        void setCollisionShape(btCollisionShape* shape) { mCollisionShape = shape; }

        bool mAnchored = false;
        bool mCanCollide = true;

        float mMass = 1.0f;
        glm::vec3 mSize = glm::vec3(1.0f);
        glm::vec3 mCenter = glm::vec3(0.0f);

        float mFriction = 0.5f;
        float mRestitution = 0.0f;
        float mLinearDamping = 0.0f;
        float mAngularDamping = 0.05f;

        glm::vec3 mPendingVelocity = glm::vec3(0.0f);
        glm::vec3 mPendingAngularVelocity = glm::vec3(0.0f);
        glm::vec3 mPendingImpulse = glm::vec3(0.0f);

        bool mNeedsRecreate = true;
        bool mInPhysicsWorld = false; 

        btRigidBody* mRigidBody = nullptr;
        btCollisionShape* mCollisionShape = nullptr;
    };

}

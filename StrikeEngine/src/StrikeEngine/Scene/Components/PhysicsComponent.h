#pragma once
#include "Component.h"

#include <glm/glm.hpp>
#include <pugixml.hpp>

class btRigidBody;
class btCollisionShape;

namespace StrikeEngine {

    class PhysicsComponent : public Component {
    public:
        PhysicsComponent() = default;
        virtual ~PhysicsComponent();

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "physics";
            return typeName;
        }

        const std::string& getTypeName() const override { return getStaticTypeName(); }

        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // Properties
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

        // Runtime control - Linear
        void setVelocity(const glm::vec3& velocity);
        glm::vec3 getVelocity() const;

        // Runtime control - Angular 
        void setAngularVelocity(const glm::vec3& angularVel);
        glm::vec3 getAngularVelocity() const;

        void applyImpulse(const glm::vec3& impulse);

        // Material properties
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

        glm::vec3 getBulletCenterOffset() const { return mBulletCenterOffset; }

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
        glm::vec3 mBulletCenterOffset = glm::vec3(0.0f);

        float mFriction = 0.5f;
        float mRestitution = 0.0f;
        float mLinearDamping = 0.0f;
        float mAngularDamping = 0.05f;

        // Pending values (used when rigid body doesn't exist yet)
        glm::vec3 mPendingVelocity = glm::vec3(0.0f);
        glm::vec3 mPendingAngularVelocity = glm::vec3(0.0f);
        glm::vec3 mPendingImpulse = glm::vec3(0.0f);

        bool mNeedsRecreate = true;

        btRigidBody* mRigidBody = nullptr;
        btCollisionShape* mCollisionShape = nullptr;
    };

}
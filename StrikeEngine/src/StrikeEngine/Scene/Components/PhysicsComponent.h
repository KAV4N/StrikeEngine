#pragma once
#include "Component.h"
#include "StrikeEngine/Scene/Entity.h"

#include <glm/glm.hpp>
#include <pugixml.hpp>


class btRigidBody;
class btCollisionShape;

namespace StrikeEngine {

    /**
     * @brief Structure representing a raycast hit result.
     * 
     * Contains information about an entity hit by a raycast, including
     * the hit normal and distance.
     * 
     */
    struct RayHit {
        Entity entity;                      ///< The entity that was hit
        glm::vec3 normal = glm::vec3(0.0f); ///< Surface normal at hit point
        float distance = 0.0f;              ///< Distance from ray origin to hit point

        /**
         * @brief Default constructor
         */
        RayHit() = default;
        
        /**
         * @brief Check if the raycast hit anything
         * 
         * @return True if an entity was hit, false otherwise
         */
        bool hasHit() const { return entity.isValid(); }
    };

    /**
     * @brief Component for physics simulation and collision detection.
     * 
     * The PhysicsComponent provides rigid body physics using the Bullet physics engine.
     * It supports properties like mass, velocity, friction, and collision detection.
     * 
     * @note This class is non-copyable and non-movable. Physics bodies are managed by the PhysicsSystem
     */
    class PhysicsComponent : public Component {
    public:
        /**
         * @brief Default constructor
         */
        PhysicsComponent() = default;
        
        /**
         * @brief Destructor
         */
        ~PhysicsComponent() = default;

        PhysicsComponent(const PhysicsComponent&) = delete;
        PhysicsComponent& operator=(const PhysicsComponent&) = delete;
        PhysicsComponent(PhysicsComponent&&) = delete;
        PhysicsComponent& operator=(PhysicsComponent&&) = delete;

        /**
         * @brief Get the static type name for this component
         * 
         * @return Constant reference to the static type name "physics"
         */
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "physics";
            return typeName;
        }

        /**
         * @brief Get the type name of this component instance
         * 
         * @return Constant reference to the type name "physics"
         */
        const std::string& getTypeName() const override { return getStaticTypeName(); }

        /**
         * @brief Deserialize physics data from XML node
         * 
         * @param node The XML node containing physics configuration
         */
        void deserialize(const pugi::xml_node& node) override;

        /**
         * @brief Set whether the physics body is anchored (static)
         * 
         * @param anchored True for static/kinematic body, false for dynamic
         * @note Anchored bodies have infinite mass and don't respond to forces
         */
        void setAnchored(bool anchored);
        
        /**
         * @brief Check if the physics body is anchored
         * 
         * @return True if anchored, false if dynamic
         */
        bool isAnchored() const { return mAnchored; }

        /**
         * @brief Set whether this body can collide with others
         * 
         * @param canCollide True to enable collisions, false to disable
         */
        void setCanCollide(bool canCollide);
        
        /**
         * @brief Check if collision detection is enabled
         * 
         * @return True if collisions are enabled, false otherwise
         */
        bool canCollide() const { return mCanCollide; }

        /**
         * @brief Set the mass of the physics body
         * 
         * @param mass Mass in kilograms
         * @note Mass is ignored for anchored bodies
         */
        void setMass(float mass);
        
        /**
         * @brief Get the mass of the physics body
         * 
         * @return Mass in kilograms
         */
        float getMass() const { return mMass; }

        /**
         * @brief Set the collision box size
         * 
         * @param size Collision box dimensions in world units
         */
        void setSize(const glm::vec3& size);
        
        /**
         * @brief Get the collision box size
         * 
         * @return Constant reference to collision box dimensions
         */
        const glm::vec3& getSize() const { return mSize; }

        /**
         * @brief Set the collision box center offset
         * 
         * @param center Local offset from entity position
         */
        void setCenter(const glm::vec3& center);
        
        /**
         * @brief Get the collision box center offset
         * 
         * @return Constant reference to center offset
         */
        const glm::vec3& getCenter() const { return mCenter; }

        /**
         * @brief Set the linear velocity
         * 
         * @param velocity Velocity vector in world space
         */
        void setVelocity(const glm::vec3& velocity);
        
        /**
         * @brief Get the current linear velocity
         * 
         * @return Velocity vector in world space
         */
        glm::vec3 getVelocity() const;

        /**
         * @brief Set the angular velocity
         * 
         * @param angularVel Angular velocity vector (axis-angle representation)
         */
        void setAngularVelocity(const glm::vec3& angularVel);
        
        /**
         * @brief Get the current angular velocity
         * 
         * @return Angular velocity vector
         */
        glm::vec3 getAngularVelocity() const;

        /**
         * @brief Apply an impulse to the physics body
         * 
         * @param impulse Impulse vector in world space
         * @note Impulse is applied at the center of mass
         */
        void applyImpulse(const glm::vec3& impulse);

        /**
         * @brief Set the friction coefficient
         * 
         * @param friction Friction value (typically 0.0 to 1.0)
         */
        void setFriction(float friction);
        
        /**
         * @brief Get the friction coefficient
         * 
         * @return Friction value
         */
        float getFriction() const { return mFriction; }

        /**
         * @brief Set the bounciness (restitution) coefficient
         * 
         * @param restitution Bounciness value (0.0 = no bounce, 1.0 = perfect bounce)
         */
        void setBounciness(float restitution);
        
        /**
         * @brief Get the bounciness coefficient
         * 
         * @return Bounciness value
         */
        float getBounciness() const { return mRestitution; }

        /**
         * @brief Set the linear damping factor
         * 
         * @param damping Damping coefficient (0.0 = no damping, higher = more damping)
         * @note Damping gradually reduces linear velocity over time
         */
        void setLinearDamping(float damping);
        
        /**
         * @brief Get the linear damping factor
         * 
         * @return Linear damping coefficient
         */
        float getLinearDamping() const { return mLinearDamping; }

        /**
         * @brief Set the angular damping factor
         * 
         * @param damping Damping coefficient (0.0 = no damping, higher = more damping)
         * @note Damping gradually reduces angular velocity over time
         */
        void setAngularDamping(float damping);
        
        /**
         * @brief Get the angular damping factor
         * 
         * @return Angular damping coefficient
         */
        float getAngularDamping() const { return mAngularDamping; }

        /**
         * @brief Check if the physics body needs to be recreated
         * 
         * @return True if recreation is needed, false otherwise
         * @note Used internally by the physics system
         */
        bool needsRecreate() const { return mNeedsRecreate; }
        
        /**
         * @brief Clear the recreation flag
         * 
         * @note Used internally by the physics system
         */
        void clearRecreate() { mNeedsRecreate = false; }

    private:
        friend class PhysicsSystem;

        /**
         * @brief Get the Bullet rigid body pointer (internal)
         * 
         * @return Pointer to the btRigidBody
         */
        btRigidBody* getRigidBody() const { return mRigidBody; }
        
        /**
         * @brief Set the Bullet rigid body pointer (internal)
         * 
         * @param body Pointer to the btRigidBody
         */
        void setRigidBody(btRigidBody* body) { mRigidBody = body; }

        /**
         * @brief Get the Bullet collision shape pointer (internal)
         * 
         * @return Pointer to the btCollisionShape
         */
        btCollisionShape* getCollisionShape() const { return mCollisionShape; }
        
        /**
         * @brief Set the Bullet collision shape pointer (internal)
         * 
         * @param shape Pointer to the btCollisionShape
         */
        void setCollisionShape(btCollisionShape* shape) { mCollisionShape = shape; }

        bool mAnchored = false;                        ///< Static/kinematic flag
        bool mCanCollide = true;                       ///< Collision detection flag

        float mMass = 1.0f;                            ///< Mass in kilograms
        glm::vec3 mSize = glm::vec3(1.0f);            ///< Collision box dimensions
        glm::vec3 mCenter = glm::vec3(0.0f);          ///< Collision box center offset

        float mFriction = 0.5f;                        ///< Friction coefficient
        float mRestitution = 0.0f;                     ///< Bounciness coefficient
        float mLinearDamping = 0.0f;                   ///< Linear velocity damping
        float mAngularDamping = 0.05f;                 ///< Angular velocity damping

        glm::vec3 mPendingVelocity = glm::vec3(0.0f);        ///< Pending velocity change
        glm::vec3 mPendingAngularVelocity = glm::vec3(0.0f); ///< Pending angular velocity change
        glm::vec3 mPendingImpulse = glm::vec3(0.0f);         ///< Pending impulse application

        bool mNeedsRecreate = true;                    ///< Recreation flag
        bool mInPhysicsWorld = false;                  ///< World registration flag

        btRigidBody* mRigidBody = nullptr;             ///< Bullet rigid body pointer
        btCollisionShape* mCollisionShape = nullptr;   ///< Bullet collision shape pointer
    };

}

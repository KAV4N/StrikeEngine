#pragma once
#include "SystemECS.h"

#include <glm/glm.hpp>
#include <vector>
#include <entt/entt.hpp>
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/Components/PhysicsComponent.h"

class btDiscreteDynamicsWorld;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;

namespace StrikeEngine {

    struct RayHit;

    /**
     * @brief ECS system responsible for physics simulation and collision queries.
     *
     * PhysicsSystem integrates the Bullet Physics engine with the ECS layer.
     * It manages rigid body creation, synchronization between physics and
     * transforms, collision queries, and ray casting.
     */
    class PhysicsSystem : public SystemECS {
    public:
        /**
         * @brief Constructs the physics system and initializes the physics world.
         */
        PhysicsSystem();

        /**
         * @brief Destroys the physics system and cleans up Bullet resources.
         */
        virtual ~PhysicsSystem();

        /**
         * @brief Advances the physics simulation and synchronizes entity transforms.
         *
         * Handles rigid body creation, activation/deactivation,
         * simulation stepping, and transform synchronization.
         *
         * @param dt Delta time (in seconds) since the last frame.
         */
        void onUpdate(float dt) override;

        /**
         * @brief Sets the global gravity vector.
         *
         * @param gravity Gravity acceleration in world space.
         */
        void setGravity(const glm::vec3& gravity);

        /**
         * @brief Returns the current gravity vector.
         */
        glm::vec3 getGravity() const;

        /**
         * @brief Retrieves all entities currently colliding with the given entity.
         *
         * @param entity Entity to test collisions against.
         * @return List of colliding entities.
         */
        std::vector<Entity> getCollidingEntities(const Entity& entity) const;

        /**
         * @brief Checks whether two entities are currently colliding.
         *
         * @param entityA First entity.
         * @param entityB Second entity.
         * @return True if the entities are colliding.
         */
        bool isColliding(const Entity& entityA, const Entity& entityB) const;

        /**
         * @brief Casts a ray and returns the closest hit.
         *
         * @param origin Ray start position.
         * @param direction Ray direction (normalized internally).
         * @param maxDistance Maximum ray distance.
         * @return RayHit containing hit information, if any.
         */
        RayHit rayCast(const glm::vec3& origin,
                       const glm::vec3& direction,
                       float maxDistance = 1000.0f) const;

        /**
         * @brief Casts a ray and returns all hits along the ray.
         *
         * Results are sorted by distance.
         *
         * @param origin Ray start position.
         * @param direction Ray direction (normalized internally).
         * @param maxDistance Maximum ray distance.
         * @return List of ray hit results.
         */
        std::vector<RayHit> rayCastAll(const glm::vec3& origin,
                                       const glm::vec3& direction,
                                       float maxDistance = 1000.0f) const;

    private:
        friend class World;
        friend class Scene;
        friend class Entity;

        /**
         * @brief Initializes the Bullet physics world.
         */
        void createPhysicsWorld();

        /**
         * @brief Cleans up the Bullet physics world and all rigid bodies.
         */
        void cleanupPhysicsWorld();

        /**
         * @brief Creates a rigid body for an entity.
         *
         * @param entity Entity handle.
         */
        void createRigidBody(entt::entity entity);

        /**
         * @brief Removes and destroys an entity's physics representation.
         *
         * @param entity Entity handle.
         */
        void removePhysics(entt::entity entity);

        /**
         * @brief Recreates a rigid body when physics parameters change.
         *
         * @param entity Entity handle.
         */
        void recreatePhysicsBody(entt::entity entity);

        /**
         * @brief Applies deferred physics values (velocity, impulse, etc).
         *
         * @param physics Physics component.
         */
        void applyPendingValues(PhysicsComponent& physics);

        /**
         * @brief Syncs transform data from physics to ECS.
         *
         * @param entity Entity handle.
         * @param physics Physics component.
         * @param ent Entity wrapper.
         */
        void syncTransformFromPhysics(entt::entity entity,
                                      PhysicsComponent& physics,
                                      Entity& ent);

        /**
         * @brief Syncs transform data from ECS to physics.
         *
         * Used for kinematic / anchored entities.
         *
         * @param entity Entity handle.
         * @param physics Physics component.
         * @param ent Entity wrapper.
         */
        void syncTransformToPhysics(entt::entity entity,
                                    PhysicsComponent& physics,
                                    Entity& ent);

        /**
         * @brief Retrieves the ECS entity associated with a Bullet rigid body.
         *
         * @param body Bullet rigid body.
         * @return Corresponding ECS entity.
         */
        Entity getEntityFromRigidBody(const btRigidBody* body) const;

    private:
        btDiscreteDynamicsWorld* mDynamicsWorld = nullptr;
        btBroadphaseInterface* mBroadphase = nullptr;
        btDefaultCollisionConfiguration* mCollisionConfiguration = nullptr;
        btCollisionDispatcher* mDispatcher = nullptr;
        btSequentialImpulseConstraintSolver* mSolver = nullptr;

        glm::vec3 mGravity = glm::vec3(0.0f, -9.81f, 0.0f);
    };

} 

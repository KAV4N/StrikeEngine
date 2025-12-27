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

    class PhysicsSystem : public SystemECS {
    public:
        PhysicsSystem();
        virtual ~PhysicsSystem();

        void onUpdate(float dt) override;

        void setGravity(const glm::vec3& gravity);
        glm::vec3 getGravity() const;

        std::vector<Entity> getCollidingEntities(const Entity& entity) const;
        bool isColliding(const Entity& entityA, const Entity& entityB) const;

        RayHit rayCast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance = 1000.0f) const;
        std::vector<RayHit> rayCastAll(const glm::vec3& origin, const glm::vec3& direction, float maxDistance = 1000.0f) const;

    private:
        friend class World;
        friend class Scene;
        friend class Entity;

        void createPhysicsWorld();
        void cleanupPhysicsWorld();

        void createRigidBody(entt::entity entity);
        void removePhysics(entt::entity entity);
        void recreatePhysicsBody(entt::entity entity);

        void applyPendingValues(PhysicsComponent& physics);

        void syncTransformFromPhysics(entt::entity entity, PhysicsComponent& physics, Entity& ent);
        void syncTransformToPhysics(entt::entity entity, PhysicsComponent& physics, Entity& ent);

        Entity getEntityFromRigidBody(const btRigidBody* body) const;

    private:
        btDiscreteDynamicsWorld* mDynamicsWorld = nullptr;
        btBroadphaseInterface* mBroadphase = nullptr;
        btDefaultCollisionConfiguration* mCollisionConfiguration = nullptr;
        btCollisionDispatcher* mDispatcher = nullptr;
        btSequentialImpulseConstraintSolver* mSolver = nullptr;

        glm::vec3 mGravity = glm::vec3(0.0f, -9.81f, 0.0f);
    };

} // namespace StrikeEngine
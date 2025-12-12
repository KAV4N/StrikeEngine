#pragma once
#include "SystemECS.h"

#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <entt/entt.hpp>

class btDiscreteDynamicsWorld;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btRigidBody;
class btCollisionShape;

namespace StrikeEngine {

    class Scene;
    class Entity;
    class PhysicsComponent;

    class PhysicsSystem : public SystemECS {
    public:
        PhysicsSystem();
        virtual ~PhysicsSystem();

        void onUpdate(float dt) override;

        void setGravity(const glm::vec3& gravity);
        glm::vec3 getGravity() const;

    private:
        friend class World;
        friend class Scene;

        void recreatePhysicsBody(entt::entity entity);
        void syncTransformFromPhysics(entt::entity entity, PhysicsComponent& physics, Entity& ent);
        void syncTransformToPhysics(entt::entity entity, PhysicsComponent& physics, Entity& ent);

        void createPhysicsWorld();
        void cleanupPhysicsWorld();

        void createRigidBody(entt::entity entity);
        void removePhysics(entt::entity entity);
        
        void applyPendingValues(PhysicsComponent& physics);

    private:
        btDiscreteDynamicsWorld* mDynamicsWorld = nullptr;
        btBroadphaseInterface* mBroadphase = nullptr;
        btDefaultCollisionConfiguration* mCollisionConfiguration = nullptr;
        btCollisionDispatcher* mDispatcher = nullptr;
        btSequentialImpulseConstraintSolver* mSolver = nullptr;

        std::unordered_map<entt::entity, btCollisionShape*> mCollisionShapes;

        glm::vec3 mGravity = glm::vec3(0.0f, -9.81f, 0.0f);
    };

} // namespace StrikeEngine
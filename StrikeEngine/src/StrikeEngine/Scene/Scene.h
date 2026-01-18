#pragma once

#include "StrikeEngine/Events/Event.h"

#include "GraphNode.h"
#include "Sun.h"

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <unordered_map>

namespace StrikeEngine {

    class Asset;
    class CubeMap;
    class Entity;
    class PhysicsSystem;

    class Scene final {
    public:
        Scene(const std::string& id, const std::filesystem::path& path);
        ~Scene();

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        // Entity creation and retrieval
        Entity createEntity();
        Entity createEntity(const Entity& parent);
        Entity getEntityFromHandle(entt::entity handle);
        Entity getEntity(std::string tag);

        // Hierarchy operations
        void setParent(const Entity& child, const Entity& parent);
        void addChild(const Entity& parent, const Entity& child);

        // Skybox management
        void setSkybox(const std::string& cubeMapId);
        bool hasSkybox() const;
        const std::string& getSkyboxId() const { return mSkyboxCubeMapId; }
        std::shared_ptr<CubeMap> getSkybox() const;

        // Update operations
        void updateTransforms();

        // Management
        void shutdown();
        void destroy(entt::entity entity);

        bool isAncestor(const Entity& ancestor, const Entity& descendant) const;
        bool isDescendant(const Entity& descendant, const Entity& ancestor) const;

        entt::registry& getRegistry() { return mRegistry; }
        const entt::registry& getRegistry() const { return mRegistry; }

        template<typename Component, typename... Args>
        decltype(auto) addComponent(entt::entity entity, Args&&... args) {
            return mRegistry.emplace<Component>(entity, std::forward<Args>(args)...);
        }

        template<typename Component>
        void removeComponent(entt::entity entity) {
            mRegistry.remove<Component>(entity);
        }

        template<typename Component>
        Component& getComponent(entt::entity entity) {
            return mRegistry.get<Component>(entity);
        }

        template<typename Component>
        const Component& getComponent(entt::entity entity) const {
            return mRegistry.get<Component>(entity);
        }

        template<typename Component>
        bool hasComponent(entt::entity entity) const {
            return mRegistry.all_of<Component>(entity);
        }

        template<typename... Components>
        auto view() {
            return mRegistry.view<Components...>();
        }

        template<typename... Components>
        auto view() const {
            return mRegistry.view<Components...>();
        }

        void onUpdate(float dt);
        void onRender();

        Sun& getSun() { return mSun; }
        const Sun& getSun() const { return mSun; }
        const std::string& getId() { return mId; }

    private:
        friend class Entity;
        friend class World;
        
        // Internal graph node access (for Entity class only)
        std::shared_ptr<GraphNode> getGraphNode(entt::entity entity);
        const std::shared_ptr<GraphNode> getGraphNode(entt::entity entity) const;

        void updateNodeTransforms(std::shared_ptr<GraphNode> node, bool parentDirty = false);

        void onPhysicsComponentDestroy(entt::registry& registry, entt::entity entity);
        void onPhysicsComponentCreate(entt::registry& registry, entt::entity entity);
        void setupComponentProtection();

        void setPhysicsSystem(PhysicsSystem* physicsSystem);

    private:
        std::string mId;
        entt::registry mRegistry;
        Sun mSun; 

        std::string mSkyboxCubeMapId;

        // Graph node storage
        std::unordered_map<entt::entity, std::shared_ptr<GraphNode>> mGraphNodes;

        PhysicsSystem* mPhysicsSystem = nullptr;
    };

} // namespace StrikeEngine
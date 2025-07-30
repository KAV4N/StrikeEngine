#pragma once
#include "Entity.h"
#include <entt/entt.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace StrikeEngine {
    class Scene {
    public:
        Scene(const std::string& sceneId, const std::string& name = "");
        ~Scene() = default;

        const std::string& GetSceneId() const { return m_sceneId; }
        const std::string& GetName() const { return m_name; }

        std::shared_ptr<Entity> CreateEntity(const std::string& name = "", const std::string& id = "");
        void RemoveEntity(std::shared_ptr<Entity> entity);
        void RemoveEntity(const std::string& id);

        std::shared_ptr<Entity> GetEntity(const std::string& id) const;
        std::shared_ptr<Entity> GetEntityByHandle(entt::entity handle) const;
        const std::vector<std::shared_ptr<Entity>>& GetEntities() const { return m_entities; }
        const std::vector<std::shared_ptr<Entity>>& GetRootEntities() const { return m_rootEntities; }

        entt::registry& GetRegistry() { return m_registry; }
        const entt::registry& GetRegistry() const { return m_registry; }

        void Update(float deltaTime);

    private:
        std::string m_sceneId;
        std::string m_name;
        entt::registry m_registry;
        std::vector<std::shared_ptr<Entity>> m_entities;
        std::vector<std::shared_ptr<Entity>> m_rootEntities;
        std::unordered_map<std::string, std::shared_ptr<Entity>> m_entitiesById;
        std::unordered_map<entt::entity, std::shared_ptr<Entity>> m_entityMap;
    };
}
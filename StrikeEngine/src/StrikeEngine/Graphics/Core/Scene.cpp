#include "Scene.h"
#include <algorithm>

namespace StrikeEngine {
    Scene::Scene(const std::string& sceneId, const std::string& name)
        : m_sceneId(sceneId), m_name(name) {
    }

    std::shared_ptr<Entity> Scene::CreateEntity(const std::string& name, const std::string& id) {
        auto entity = std::make_shared<Entity>(this, m_registry, id, name);
        m_entities.push_back(entity);

        if (!id.empty()) {
            m_entitiesById[id] = entity;
        }

        m_entityMap[entity->GetHandle()] = entity;
        m_rootEntities.push_back(entity);

        return entity;
    }

    void Scene::RemoveEntity(std::shared_ptr<Entity> entity) {
        if (!entity) return;

        auto it = std::find(m_entities.begin(), m_entities.end(), entity);
        if (it != m_entities.end()) {
            m_entities.erase(it);
        }

        auto rootIt = std::find(m_rootEntities.begin(), m_rootEntities.end(), entity);
        if (rootIt != m_rootEntities.end()) {
            m_rootEntities.erase(rootIt);
        }

        if (!entity->GetId().empty()) {
            m_entitiesById.erase(entity->GetId());
        }

        m_entityMap.erase(entity->GetHandle());
    }

    void Scene::RemoveEntity(const std::string& id) {
        auto entity = GetEntity(id);
        if (entity) {
            RemoveEntity(entity);
        }
    }

    std::shared_ptr<Entity> Scene::GetEntity(const std::string& id) const {
        auto it = m_entitiesById.find(id);
        return (it != m_entitiesById.end()) ? it->second : nullptr;
    }

    std::shared_ptr<Entity> Scene::GetEntityByHandle(entt::entity handle) const {
        auto it = m_entityMap.find(handle);
        return (it != m_entityMap.end()) ? it->second : nullptr;
    }

    void Scene::Update(float deltaTime) {
        // Update scene systems here
    }
}
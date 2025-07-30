#pragma once
#include "GraphNode.h"
#include <entt/entt.hpp>
#include <string>
#include <memory>
#include <vector>

namespace StrikeEngine {
    class Scene; // Forward declaration

    class Entity : public GraphNode {
    public:
        Entity(Scene* scene, entt::registry& registry, const std::string& id, const std::string& name = "");
        ~Entity();

        entt::entity GetHandle() const { return m_entityHandle; }
        entt::registry& GetRegistry() const { return m_registry; }

        // Component management
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            return m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent() {
            return m_registry.get<T>(m_entityHandle);
        }

        template<typename T>
        const T& GetComponent() const {
            return m_registry.get<T>(m_entityHandle);
        }

        template<typename T>
        bool HasComponent() const {
            return m_registry.all_of<T>(m_entityHandle);
        }

        template<typename T>
        void RemoveComponent() {
            m_registry.remove<T>(m_entityHandle);
        }

        // Entity hierarchy
        void AddChildEntity(entt::entity childId);
        void RemoveChildEntity(entt::entity childId);
        std::shared_ptr<Entity> GetParentEntity() const;
        std::vector<std::shared_ptr<Entity>> GetChildEntities() const;

        // Utility
        bool IsValid() const { return m_registry.valid(m_entityHandle); }

    private:
        entt::registry& m_registry;
        entt::entity m_entityHandle;
    };
}
#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "StrikeEngine/Graphics/Core/Model.h"
#include <vector>
#include <iostream>

namespace StrikeEngine {

    class Scene;
    class SceneGraph;

    class Entity {
    public:
        Entity(entt::entity handle = entt::null, Scene* scene = nullptr);

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template<typename T>
        void RemoveComponent() {
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }

        template<typename T>
        T& GetComponent() const {
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

        template<typename T>
        T& GetComponent() {
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

        template<typename T>
        bool HasComponent() const {
            return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
        }

        template<typename T, typename... Args>
        T& GetOrAddComponent(Args&&... args) {
            if (HasComponent<T>()) {
                return GetComponent<T>();
            }
            return AddComponent<T>(std::forward<Args>(args)...);
        }


        bool SetParent(Entity parent);
        Entity GetParent() const;
        bool AddChild(Entity child);
        bool RemoveChild(Entity child);
        std::vector<Entity> GetChildren() const;
        std::vector<Entity> GetAllDescendants() const;
        bool Destroy();

        bool operator==(const Entity& other) const;
        bool operator!=(const Entity& other) const;
        operator bool() const;

        inline entt::entity GetHandle() const { return m_EntityHandle; }
        inline Scene* GetScene() const { return m_Scene; }

    private:
        entt::entity m_EntityHandle;
        Scene* m_Scene;

        friend class SceneGraph;
    };

}

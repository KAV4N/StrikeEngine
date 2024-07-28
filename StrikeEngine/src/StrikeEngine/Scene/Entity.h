#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "StrikeEngine/Renderer/Model.h"
#include "StrikeEngine/Scene/Components/TransformComponents.h"

namespace StrikeEngine {

    class Scene;

    class Entity {
    public:
        Entity(entt::entity handle, Scene* scene)
            : m_EntityHandle(handle), m_Scene(scene) {}

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template<typename T>
        void RemoveComponent() {
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }

        template<typename T>
        T& GetComponent() {
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }

        template<typename T>
        bool HasComponent() const {
            return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
        }

        entt::entity GetHandle() const { return m_EntityHandle; }

    private:
        entt::entity m_EntityHandle;
        Scene* m_Scene;
    };

}
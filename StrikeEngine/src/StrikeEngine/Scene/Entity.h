#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include "StrikeEngine/Graphics/Core/Model.h"


namespace StrikeEngine {

    class Scene;

    class Entity {
    public:

        Entity(entt::entity handle = entt::null, Scene* scene = nullptr)
            : m_EntityHandle(handle), m_Scene(scene) {}

        void Set(Entity entity) {
            m_EntityHandle = entity.GetHandle();
            m_Scene = entity.GetScene();
        }

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

        inline entt::entity GetHandle() const { return m_EntityHandle; }
        inline Scene* GetScene() const { return m_Scene; }

    private:
        entt::entity m_EntityHandle;
        Scene* m_Scene;
    };

}
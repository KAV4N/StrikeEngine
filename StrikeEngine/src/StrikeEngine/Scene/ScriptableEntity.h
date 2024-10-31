#pragma once
#include <iostream>
#include <entt/entt.hpp>

namespace StrikeEngine {

    class Entity;

    enum class ScriptState {
        Uninitialized,
        Enabled,
        Disabled
    };

    class ScriptableEntity {
    public:
        ScriptableEntity() = default;

        ~ScriptableEntity() = default;


        ScriptableEntity(const ScriptableEntity&) = delete;
        ScriptableEntity& operator=(const ScriptableEntity&) = delete;


        virtual void EnableScript() final {
            if (m_State == ScriptState::Disabled) {
                m_State = ScriptState::Enabled;
                OnScriptEnable();
            }
        }

        virtual void DisableScript() final {
            if (m_State == ScriptState::Enabled) {
                m_State = ScriptState::Disabled;
                OnScriptDisable();
            }
        }

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            return m_Entity.AddComponent<T>(std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent() {
            return m_Entity.GetComponent<T>();
        }

        template<typename T>
        bool HasComponent() const {
            return m_Entity.HasComponent<T>();
        }

        template<typename T>
        void RemoveComponent() {
            m_Entity.RemoveComponent<T>();
        }

        Entity GetParent() const { return m_Entity.GetParent(); }
        void SetParent(Entity parent) { m_Entity.SetParent(parent); }

        std::vector<Entity> GetChildren() const { return m_Entity.GetChildren(); }

        Entity& GetEntity() { return m_Entity; }
        const Entity& GetEntity() const { return m_Entity; }

        const virtual ScriptState GetEntityState() const final {
            return m_State;
        }

    protected:
        virtual void OnScriptStart() { }
        virtual void OnScriptUpdate(float t_DeltaTime) { }
        virtual void OnScriptDestroy() { }
        virtual void OnScriptEnable() { }
        virtual void OnScriptDisable() { }

    private:

        void StartScript(entt::entity entity, Scene* scene) {
            if (m_State == ScriptState::Uninitialized) {
                m_State = ScriptState::Enabled;
                m_Entity = Entity{ entity, scene };
                OnScriptStart();
            }
        }

        void UpdateScript(float t_DeltaTime) {
            if (m_State == ScriptState::Enabled) {
                OnScriptUpdate(t_DeltaTime);
            }
        }

    private:

        ScriptState m_State = ScriptState::Uninitialized;
        Entity m_Entity;


        friend class Scene;
        friend class NativeScriptComponent;
    };
}

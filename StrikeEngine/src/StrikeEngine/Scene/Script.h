#pragma once

#include "Entity.h"
#include <StrikeEngine/Events/Event.h>

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <typeindex>
#include <unordered_map>

namespace StrikeEngine {
    class Event;
    class KeyPressedEvent;
    class KeyReleasedEvent;
    class KeyTypedEvent;
    class MouseMovedEvent;
    class MouseScrolledEvent;
    class MouseButtonPressedEvent;
    class MouseButtonReleasedEvent;

    class Script {
    public:
        Script();
        virtual ~Script();

        Script(const Script&) = delete;
        Script& operator=(const Script&) = delete;
        Script(Script&&) = delete;
        Script& operator=(Script&&) = delete;

        virtual void onCreate();
        virtual void onStart();
        virtual void onUpdate(float deltaTime);
        virtual void onDestroy();
        
        void onEvent(Event& event);

        // Override these to handle specific events 
        virtual void onKeyPressed(KeyPressedEvent& event) {}
        virtual void onKeyReleased(KeyReleasedEvent& event) {}
        virtual void onKeyTyped(KeyTypedEvent& event) {}
        virtual void onMouseMoved(MouseMovedEvent& event) {}
        virtual void onMouseScrolled(MouseScrolledEvent& event) {}
        virtual void onMouseButtonPressed(MouseButtonPressedEvent& event) {}
        virtual void onMouseButtonReleased(MouseButtonReleasedEvent& event) {}

        const Entity& getEntity() const;

        bool isActive() const;
        void setActive(bool active);

        bool isStarted() const;

        template<typename T, typename... Args>
        T& addComponent(Args&&... args) {
            return mEntity.addComponent<T>(std::forward<Args>(args)...);
        }

        template<typename T>
        T& getComponent() {
            return mEntity.getComponent<T>();
        }

        template<typename T>
        const T& getComponent() const {
            return mEntity.getComponent<T>();
        }

        template<typename T>
        bool hasComponent() const {
            return mEntity.hasComponent<T>();
        }

        template<typename T>
        void removeComponent() {
            mEntity.removeComponent<T>();
        }

        template<typename T, typename... Args>
        T& getOrAddComponent(Args&&... args) {
            return mEntity.getOrAddComponent<T>(std::forward<Args>(args)...);
        }

    protected:
        bool mActive = true;

    private:
        friend class ScriptComponent;
        friend class ScriptSystem;

        void setEntity(const Entity& entity);
        void markStarted();

    private:
        bool mStarted = false;
        Entity mEntity;
    };
}
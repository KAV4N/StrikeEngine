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
        ~Script();

        Script(const Script&) = delete;
        Script& operator=(const Script&) = delete;
        Script(Script&&) = delete;
        Script& operator=(Script&&) = delete;

        virtual void onCreate();
        virtual void onStart();
        virtual void onUpdate(float deltaTime);
        virtual void onDestroy();
        virtual void onEvent(Event& event);
        

        const Entity& getEntity() const;

        bool isActive() const;
        void setActive(bool active);

        bool isStarted() const;

        // Timing utility - returns true every N seconds
        bool tick(float seconds);

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

        void internalUpdate(float deltaTime);

        void setEntity(const Entity& entity);
        void markStarted();
        void updateTimers(float deltaTime);

    private:
        bool mStarted = false;
        Entity mEntity;
        
        std::unordered_map<float, float> mTimers;
    };
}
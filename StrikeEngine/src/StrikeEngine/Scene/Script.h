#pragma once
#include <string>
#include <memory>
#include <functional>
#include "Entity.h"
#include <any>
#include <StrikeEngine/Events/Event.h>

namespace StrikeEngine {
    class Entity;
    class Event;
    class UserEvent;
    class EventDispatcher;

    class Script {
    public:
        Script();
        virtual ~Script();

        // Core script lifecycle methods
        virtual void onCreate();
        virtual void onStart();
        virtual void onUpdate(float deltaTime);
        virtual void onDestroy();
        virtual void onEvent(Event& event); // Now returns void

        void sendUserEvent(const std::string& eventName, std::any data = {});

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

        template<typename EventT, typename T>
        void bindEvent(T& obj, bool (T::* method)(EventT&)) {
            auto boundHandler = std::bind(method, &obj, std::placeholders::_1);
            mEventDispatcher->dispatch<EventT>(boundHandler);
        }

    protected:
        bool mActive = true;

    private:
        friend class ScriptComponent;
        friend class ScriptSystem;

        void setEntity(const Entity& entity);
        void markStarted();
        void setEventDispatcher(EventDispatcher* dispatcher);

    private:
        bool mStarted = false;
        Entity mEntity;
        EventDispatcher* mEventDispatcher = nullptr;
    };
}

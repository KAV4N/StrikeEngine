#pragma once
#include <string>
#include <memory>
#include "Entity.h"

namespace StrikeEngine {
    class Entity;

    class Script {
    public:
        Script();
        virtual ~Script();

        // Core script lifecycle methods
        virtual void onCreate();
        virtual void onStart();
        virtual void onUpdate(float deltaTime);
        virtual void onDestroy();

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

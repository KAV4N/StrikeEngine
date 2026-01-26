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


    /**
     * @brief Base class for user-defined scripts attached to entities.
     * @note Scripts can override lifecycle methods to implement custom behavior.
     */
    class Script {
    public:
        Script();
        ~Script();

        Script(const Script&) = delete;
        Script& operator=(const Script&) = delete;
        Script(Script&&) = delete;
        Script& operator=(Script&&) = delete;

        /**
         * @brief Called when the script is created.
         * @note Override to implement custom creation logic.
         */
        virtual void onCreate();

        /**
         * @brief Called when the script is started (first update).
         * @note Override to implement custom start logic.
         */
        virtual void onStart();

        /**
         * @brief Called every frame to update the script.
         * @param deltaTime Time elapsed since the last frame in seconds.
         */
        virtual void onUpdate(float deltaTime);

        /**
         * @brief Called when the script is destroyed.
         * @note Override to implement custom destruction logic.
         */
        virtual void onDestroy();

        /**
         * @brief Called when an event is dispatched to the script.
         * @param event The event to handle.
         */
        virtual void onEvent(Event& event);
        
        /**
         * @brief Get the entity to which this script is attached.
         * @return Const reference to the Entity.
         */
        const Entity& getEntity() const;

        /**
         * @brief Check if the entity is currently active.
         * @return True if the entity is active, false otherwise.
         */
        bool isActive() const;

        /**
         * @brief Set the active state of the entity.
         * @param active True to activate the entity, false to deactivate.
         */
        void setActive(bool active);

        /**
         * @brief Check if the script has been started.
         * @return True if started, false otherwise.
         */
        bool isStarted() const;

        /**
         * @brief Set a timer with a specified duration.
         * @param duration Duration in seconds after which the timer expires.
         * @return The time remaining on the timer in seconds.
         */
        bool tick(float seconds);

        /**
         * @brief Add a component to the entity.
         * @tparam T The type of the component to add.
         * @tparam Args Constructor arguments for the component.
         * @param args Arguments to forward to the component constructor.
         * @return Reference to the added component.
         */
        template<typename T, typename... Args>
        T& addComponent(Args&&... args) {
            return mEntity.addComponent<T>(std::forward<Args>(args)...);
        }

        /**
        * @brief Get a component from the entity.
        * @tparam T The type of the component to get.
        * @return Reference to the requested component.
        */
        template<typename T>
        T& getComponent() {
            return mEntity.getComponent<T>();
        }

        /**
         * @brief Get a const component from the entity.
         * @tparam T The type of the component to get.
         */
        template<typename T>
        const T& getComponent() const {
            return mEntity.getComponent<T>();
        }

        /**
         * @brief Check if the entity has a specific component.
         * @tparam T The type of the component to check for.
         * @return True if the component exists, false otherwise.
         */
        template<typename T>
        bool hasComponent() const {
            return mEntity.hasComponent<T>();
        }

        /**
         * @brief Remove a component from the entity.
         * @tparam T The type of the component to remove.
         */
        template<typename T>
        void removeComponent() {
            mEntity.removeComponent<T>();
        }

        /**
         * @brief Get or add a component to the entity.
         * @tparam T The type of the component.
         * @tparam Args Constructor arguments for the component if it needs to be added.
         * @param args Arguments to forward to the component constructor if adding.
         * @return Reference to the existing or newly added component.
         */
        template<typename T, typename... Args>
        T& getOrAddComponent(Args&&... args) {
            return mEntity.getOrAddComponent<T>(std::forward<Args>(args)...);
        }

    protected:

        bool mActive = true; /// Whether the script's entity is active

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
#pragma once
#include "Entity.h"
#include <StrikeEngine/Events/Event.h>

#include <string>
#include <memory>
#include <unordered_map>

namespace Strike {
    class Event;

    /**
     * @brief Base class for user-defined scripts attached to entities.
     * @note Scripts can override lifecycle methods to implement custom behavior.
     */
    class Script {
    public:
        Script() = default;
        ~Script() = default;

        Script(const Script&) = delete;
        Script& operator=(const Script&) = delete;
        Script(Script&&) = delete;
        Script& operator=(Script&&) = delete;

        virtual void onStart();
        virtual void onUpdate(float deltaTime);
        virtual void onDestroy();
        virtual void onEvent(Event& event);

        /**
         * @brief Get the entity to which this script is attached.
         * @return Copy of the Entity handle.
         */
        Entity getEntity() const;

        /**
         * @brief Check if the script is currently active.
         */
        bool isActive() const;

        /**
         * @brief Set the active state of the script.
         */
        void setActive(bool active);

        /**
         * @brief Check if the script has been started.
         */
        bool isStarted() const;

        /**
         * @brief Returns true once per interval — useful for cooldowns and periodic logic.
         * @param seconds Interval duration in seconds.
         */
        bool tick(float seconds);

    protected:
        Entity& scriptEntity = mEntity;

    private:
        friend class ScriptComponent;
        friend class ScriptSystem;

        void internalUpdate(float deltaTime);
        void setEntity(const Entity& entity);
        void markStarted();

    private:
        bool mStarted = false;
        Entity mEntity;
        bool mActive = true;

        std::unordered_map<float, float> mTimers;
    };
}
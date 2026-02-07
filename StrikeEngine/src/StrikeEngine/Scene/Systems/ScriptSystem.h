#pragma once
#include "SystemECS.h"
#include <entt/entt.hpp>

namespace StrikeEngine {

    class Scene;
    class Event;

    /**
     * @brief System responsible for updating and dispatching script logic.
     *
     * ScriptSystem iterates over entities containing LogicComponent and
     * updates active scripts, handling their lifecycle callbacks such as
     * creation, start, update, and event handling.
     */
    class ScriptSystem : public SystemECS {
    public:
        /**
         * @brief Default constructor.
         */
        ScriptSystem() = default;

        /**
         * @brief Virtual destructor.
         */
        virtual ~ScriptSystem() = default;

        /**
         * @brief Updates all active scripts in the current scene.
         *
         * Handles script initialization, start callbacks, and per-frame updates.
         *
         * @param dt Delta time (in seconds) since the last frame.
         */
        void onUpdate(float dt) override;

        /**
         * @brief Dispatches an event to active scripts.
         *
         * The event is propagated until it is marked as handled.
         *
         * @param event Reference to the event being processed.
         */
        void onEvent(Event& event);
    };

}

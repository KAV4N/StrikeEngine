#pragma once

#include <entt/entt.hpp>

namespace StrikeEngine {

    /**
     * @brief Base class for all ECS systems in the engine.
     *
     * SystemECS defines the common interface for systems that operate
     * on entities and components. Derived systems can override update
     * and render callbacks to implement custom behavior.
     */
    class SystemECS {
    public:
        /**
         * @brief Default constructor.
         */
        SystemECS() = default;

        /**
         * @brief Virtual destructor.
         */
        virtual ~SystemECS() = default;

        /**
         * @brief Called every frame to update system logic.
         *
         * @param dt Delta time (in seconds) since the last frame.
         */
        virtual void onUpdate(float dt) {};

        /**
         * @brief Called every frame to handle rendering-related logic.
         */
        virtual void onRender() {};
    };

}

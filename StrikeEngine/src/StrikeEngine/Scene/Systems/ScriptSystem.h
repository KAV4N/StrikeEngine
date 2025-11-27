#pragma once
#include "SystemECS.h"
#include <entt/entt.hpp>

namespace StrikeEngine {

    class Scene;
    class Event;

    class ScriptSystem : public SystemECS {
    public:
        ScriptSystem() = default;
        virtual ~ScriptSystem() = default;

        void onUpdate(float dt) override;
        void onEvent(Event& event);

    };

} 
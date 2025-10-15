#pragma once
#include <entt/entt.hpp>
#include "StrikeEngine/Scene/Scene.h"

namespace StrikeEngine {

    class SystemECS {
    public:
        SystemECS() = default;
        virtual ~SystemECS() = default;

        virtual void onUpdate(float dt) {};

        virtual void onRender() {};

    };

}
#pragma once

#include "StrikeEngine/Scene/Scene.h"

namespace StrikeEngine {

    class ScriptSystem {
    public:
        ScriptSystem() = default;
        ~ScriptSystem() = default;

        // Update all scripts in the scene
        void onUpdate(Scene* scene, float dt);

        void onEvent(Scene* scene, Event& e);

    private:
        // Prevent copying
        ScriptSystem(const ScriptSystem&) = delete;
        ScriptSystem& operator=(const ScriptSystem&) = delete;
    };

}
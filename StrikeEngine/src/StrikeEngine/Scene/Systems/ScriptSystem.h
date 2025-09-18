#pragma once
#include "SystemECS.h"
#include "StrikeEngine/Scene/Scene.h"

namespace StrikeEngine {

    class ScriptSystem :public SystemECS {
    public:
        ScriptSystem() = default;
        ~ScriptSystem() = default;

        // Update all scripts in the scene
        void onUpdate(float dt) override;

        void onEvent(Event& e);

    private:
        // Prevent copying
        ScriptSystem(const ScriptSystem&) = delete;
        ScriptSystem& operator=(const ScriptSystem&) = delete;
    };

}
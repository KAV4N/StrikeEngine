#include "ScriptSystem.h"
#include "StrikeEngine/Scene/Components/ScriptComponent.h"

namespace StrikeEngine {

    void ScriptSystem::onEvent(Scene* scene, Event& e) {
        // Event handling logic here
    }

    void ScriptSystem::onUpdate(Scene* scene, float dt) {
        
        if (!scene || !scene->isActive()) {
            return;
        }

        auto scriptView = scene->getRegistry().view<ScriptComponent>();

        for (auto entityHandle : scriptView) {
            Entity entity = scene->getEntity(entityHandle);
            auto& scriptComponent = entity.getComponent<ScriptComponent>();

            auto& scriptInstances = scriptComponent.getScriptInstances();

            for (auto& [scriptId, script] : scriptInstances) {
                if (script) {
                    if (!script->isStarted()) {
                        script->onCreate();
                        script->onStart();
                        script->markStarted();
                    }
                    if (script->isActive()) {
                        script->onUpdate(dt);
                    }
                }
            }
        }
    }

}
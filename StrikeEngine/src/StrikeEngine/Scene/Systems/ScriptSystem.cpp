#include "ScriptSystem.h"
#include "StrikeEngine/Scene/Components/ScriptComponent.h"
#include "StrikeEngine/Scene/World.h"


namespace StrikeEngine {

    void ScriptSystem::onEvent(Event& e) {
        // Event handling logic here
    }

    void ScriptSystem::onUpdate(float dt) {
        Scene* scene = World::get().getCurrentScene();
        if (!scene) return;

        auto sceneGraph = scene->getSceneGraph();
        auto entities = sceneGraph->getEntitiesWithComponent<ScriptComponent>();

        for (auto entity : entities) {
            auto& scriptComponent = entity.getComponent<ScriptComponent>();
            if (!scriptComponent.isValid()) {
                scriptComponent.mEntity = entity;
                scriptComponent.asignEntityToScripts(entity);
            }

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
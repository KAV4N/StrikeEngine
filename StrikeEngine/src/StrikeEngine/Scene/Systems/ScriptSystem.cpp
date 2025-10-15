#include "ScriptSystem.h"
#include "StrikeEngine/Scene/Components/ScriptComponent.h"
#include "StrikeEngine/Scene/World.h"
#include "StrikeEngine/Events/Event.h"

namespace StrikeEngine {

    void ScriptSystem::onEvent(Event& e) {
        Scene* scene = World::get().getCurrentScene();
        if (!scene) return;

        auto sceneGraph = scene->getSceneGraph();
        auto entities = sceneGraph->getEntitiesWithComponent<ScriptComponent>();

        for (auto entity : entities) {
            auto& scriptComponent = entity.getComponent<ScriptComponent>();
            validateComponent(entity, scriptComponent);

            auto& scriptInstances = scriptComponent.getScriptInstances();
            for (auto& [scriptId, script] : scriptInstances) {
                // Create dispatcher for this script and event
                EventDispatcher dispatcher(e);

                script->setEventDispatcher(&dispatcher);
                script->onEvent(e);
                script->setEventDispatcher(nullptr);

                if (e.handled) {
                    break;
                }
            }
        }
    }

    void ScriptSystem::onUpdate(float dt) {
        Scene* scene = World::get().getCurrentScene();
        if (!scene) return;

        auto sceneGraph = scene->getSceneGraph();
        auto entities = sceneGraph->getEntitiesWithComponent<ScriptComponent>();

        for (auto entity : entities) {
            auto& scriptComponent = entity.getComponent<ScriptComponent>();
            validateComponent(entity, scriptComponent);

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

    void ScriptSystem::validateComponent(Entity entity, ScriptComponent& scriptComponent) {
        if (!scriptComponent.isValid()) {
            scriptComponent.mEntity = entity;
            scriptComponent.asignEntityToScripts(entity);
        }
    }
}
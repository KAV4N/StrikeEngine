#include "ScriptSystem.h"
#include "StrikeEngine/Scene/Scene.h"
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/Components/LogicComponent.h"
#include "StrikeEngine/Scene/World.h"

namespace StrikeEngine {

    void ScriptSystem::onUpdate(float dt) {
        Scene* scene = World::get().getScene();
        if (!scene) return;

        auto view = scene->view<LogicComponent>();
        
        for (auto entity : view) {
            auto& logic = view.get<LogicComponent>(entity);
            
            if (!logic.isActive()) {
                continue;
            }

            Entity entityWrapper(entity, scene);

            for (auto& script : logic.getScripts()) {
                if (!script || !script->isActive()) {
                    continue;
                }

                // Set entity if not already set
                if (!script->getEntity().isValid()) {
                    script->setEntity(entityWrapper);
                    script->onCreate();
                }

                // Call onStart once
                if (!script->isStarted()) {
                    script->onStart();
                    script->markStarted();
                }

                script->internalUpdate(dt);
            }
        }
    }

    void ScriptSystem::onEvent(Event& event) {
        Scene* scene = World::get().getScene();
        if (!scene) return;

        auto view = scene->view<LogicComponent>();
        
        for (auto entity : view) {
            if (event.handled){
                break;
            }
            auto& logic = view.get<LogicComponent>(entity);
            
            if (!logic.isActive()) {
                continue;
            }

            for (auto& script : logic.getScripts()) {
                if (script && script->isActive() && script->isStarted()) {
                    script->onEvent(event);
                    if (event.handled){
                        break;
                    }
                }
            }
        }
    }
}
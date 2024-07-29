#pragma once
#include <StrikeEngine/Renderer/Renderer.h>
#include <StrikeEngine/Scene/Components/LightComponent.h>

namespace StrikeEngine {
    class LightRenderSystem {
    public:
        static void Render(Scene* scene, Renderer* renderer) {
            auto view = scene->GetRegistry().view<LightComponent, TransformComponent>();
            for (auto entity : view) {
                auto [light, transform] = view.get<LightComponent, TransformComponent>(entity);
                if (light.visualModel) {
                    glm::mat4 lightTransform = transform.transformationMatrix;
                    lightTransform = glm::scale(lightTransform, glm::vec3(light.visualScale));
                    renderer->SubmitModel(light.visualModel, lightTransform);
                }
            }
        }
    };
}
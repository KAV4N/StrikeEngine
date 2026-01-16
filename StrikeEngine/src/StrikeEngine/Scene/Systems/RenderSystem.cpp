#include "strikepch.h"
#include "RenderSystem.h"

#include "StrikeEngine/Scene/Components/RendererComponent.h"
#include "StrikeEngine/Scene/Components/LightComponent.h"
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include "StrikeEngine/Scene/Components/TextComponent.h"

#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/World.h"
#include "StrikeEngine/Scene/Scene.h"

#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Texture.h"

#include "StrikeEngine/Graphics/Renderer.h"
#include "StrikeEngine/Graphics/FontRenderer.h"
#include "StrikeEngine/Core/Profiler.h"

namespace StrikeEngine {

    RenderSystem::RenderSystem() {}

    void RenderSystem::onUpdate(float dt) {
        PROFILE_SCOPE("RenderSystem::onUpdate");
        Scene* scene = World::get().getScene();
        if (!scene) return;

        processScene(scene);
    }

    void RenderSystem::onRender() {
        Scene* scene = World::get().getScene();
        if (!scene) return;
        
        Renderer::get().display();

        auto view = scene->view<TextComponent>();
        for (auto entity : view) {
            auto& textComp = view.get<TextComponent>(entity);
            Entity ent(entity, scene);
            if (!textComp.isActive() || !ent.isActive()) continue;

            glm::vec3 position = ent.getPosition();
            glm::vec3 scale = ent.getScale();

            FontRenderer::get().renderText(
                textComp.getText(),
                position.x,
                position.y,
                scale.x,
                textComp.getColor(),
                textComp.getPivot()
            );
        }
    }

    void RenderSystem::resize(uint32_t width, uint32_t height) {
        PROFILE_SCOPE("RenderSystem::resize");

        Scene* scene = World::get().getScene();
        if (!scene) return;

        auto& renderer = Renderer::get();
        auto& registry = scene->getRegistry();
        auto view = registry.view<CameraComponent>();
        
        for (auto entity : view) {
            Entity ent(entity, scene);
            auto& camera = registry.get<CameraComponent>(entity);
            glm::mat4 worldMatrix = ent.getWorldMatrix();
            camera.update(worldMatrix, renderer.getWidth(), renderer.getHeight());
        }
    }

    void RenderSystem::processScene(Scene* scene) {
        PROFILE_SCOPE("RenderSystem::processScene");
        auto& registry = scene->getRegistry();
        auto& renderer = Renderer::get();
        std::multimap<int, CameraRenderData> cameras;

        auto skybox = scene->getSkyboxCubeMap(); 
        auto view = registry.view<CameraComponent>();
        
        for (auto entity : view) {
            Entity ent(entity, scene);
            if (!ent.isActive()) continue;

            auto& camera = registry.get<CameraComponent>(entity);

            CameraRenderData cameraRenderData;
            cameraRenderData.camera = camera;
            cameraRenderData.cameraPosition = ent.getPosition();

            cameras.emplace(camera.getRenderOrder(), cameraRenderData);
        }

        for (const auto& [renderOrder, camData] : cameras) {
            renderer.beginCamera(camData.camera, camData.cameraPosition);

            if (skybox) renderer.submitSkybox(skybox); 
            processLights(scene);
            processRenderables(scene);

            renderer.endCamera();
        }
    }

    void RenderSystem::processLights(Scene* scene) {
        PROFILE_SCOPE("RenderSystem::processLights");

        auto& registry = scene->getRegistry();
        auto& renderer = Renderer::get();

        renderer.submitSun(&scene->getSun());    

        auto pointLightView = registry.view<LightComponent>();
        for (auto entity : pointLightView) {
            Entity ent(entity, scene);
            auto& light = registry.get<LightComponent>(entity);

            if (!ent.isActive() || !light.isActive()) continue;
            glm::vec3 position = ent.getPosition();

            submitPointLight(position, light.getColor(), light.getIntensity(),
                           light.getRadius());
        }
    }

    void RenderSystem::processRenderables(Scene* scene) {
        PROFILE_SCOPE("RenderSystem::processRenderables");

        auto& registry = scene->getRegistry();
        auto view = registry.view<RendererComponent>();

        for (auto entity : view) {
            Entity ent(entity, scene);
            auto& rendererComp = registry.get<RendererComponent>(entity);

            if (!ent.isActive() || !rendererComp.isActive()) continue;

            glm::mat4 worldMatrix = ent.getWorldMatrix();
            auto texture = rendererComp.getTexture();
            glm::vec4 color = rendererComp.getColor();

            if (rendererComp.hasMesh()) {
                auto mesh = rendererComp.getMesh();
                if (mesh) {
                    submitMesh(mesh, texture, color, worldMatrix);
                }
            } else if (rendererComp.hasModel()) {
                auto model = rendererComp.getModel();
                if (model) {
                    submitModel(model, texture, color, worldMatrix);
                }
            }
        }
    }

    void RenderSystem::submitMesh(const std::shared_ptr<Mesh>& mesh,
                                  const std::shared_ptr<Texture>& texture,
                                  const glm::vec4& color,
                                  const glm::mat4& transform) {
        Renderer::get().submitMesh(mesh, texture, color, transform);
    }

    void RenderSystem::submitModel(const std::shared_ptr<Model>& model,
                                   const std::shared_ptr<Texture>& texture,
                                   const glm::vec4& color,
                                   const glm::mat4& transform) {
        Renderer::get().submitModel(model, texture, color, transform);
    }

    void RenderSystem::submitPointLight(const glm::vec3& position,
                                       const glm::vec3& color,
                                       float intensity,
                                       float radius
                                       ) {
        Renderer::get().submitPointLight(position, color, intensity, radius);
    }
}
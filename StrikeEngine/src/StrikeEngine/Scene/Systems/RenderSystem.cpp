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
#include "StrikeEngine/Asset/Types/Material.h"

#include "StrikeEngine/Graphics/Renderer.h"
#include "StrikeEngine/Graphics/FontRenderer.h"
#include "StrikeEngine/Core/Profiler.h"

namespace StrikeEngine {

    RenderSystem::RenderSystem() {}

    void RenderSystem::onUpdate(float dt)
    {
        PROFILE_SCOPE("RenderSystem::onUpdate");
        Scene* scene = World::get().getScene();
        if (!scene) return;

        // Process all cameras and render them
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
            glm::vec3 position = ent.getPosition();
            glm::vec3 scale = ent.getScale();
            if (!textComp.isActive()) continue;

            FontRenderer::get().renderText(
                textComp.getText(),
                position.x,
                position.y,
                scale.x,
                textComp.getColor()
            );
        }

    }

    void RenderSystem::resize(uint32_t width, uint32_t height)
    {
        PROFILE_SCOPE("RenderSystem::resize");

        Scene* scene = World::get().getScene();
        if (!scene) return;

        auto& renderer = Renderer::get();

        auto& registry = scene->getRegistry();
        auto view = registry.view<CameraComponent>();
        
        for (auto entity : view)
        {
            Entity ent(entity, scene);
            auto& camera = registry.get<CameraComponent>(entity);
            glm::mat4 worldMatrix = ent.getWorldMatrix();
            camera.update(worldMatrix, renderer.getWidth(), renderer.getHeight());
        }
    }

    void RenderSystem::processScene(Scene* scene)
    {
        PROFILE_SCOPE("RenderSystem::processScene");
        // Process cameras in render order
        processCameras(scene);
    }

    void RenderSystem::processCameras(Scene* scene)
    {
        PROFILE_SCOPE("RenderSystem::processCameras");

        auto& registry = scene->getRegistry();
        auto& renderer = Renderer::get();

        // Collect and sort cameras
        struct CameraData {
            Entity entity;
            CameraComponent* camera;
            glm::mat4 worldMatrix;
            glm::vec3 position;
            int renderOrder;
        };

        std::vector<CameraData> cameras;
        auto view = registry.view<CameraComponent>();
        
        for (auto entity : view)
        {
            Entity ent(entity, scene);
            auto& camera = registry.get<CameraComponent>(entity);
            glm::mat4 worldMatrix = ent.getWorldMatrix();
            
            cameras.push_back({
                ent,
                &camera,
                worldMatrix,
                glm::vec3(worldMatrix[3]),
                camera.getRenderOrder()
            });
        }

        // Sort by render order
        std::sort(cameras.begin(), cameras.end(),
            [](const CameraData& a, const CameraData& b) {
                return a.renderOrder < b.renderOrder;
            });

        // Process each camera
        for (const auto& camData : cameras)
        {
            // Begin rendering for this camera
            renderer.beginCamera(*camData.camera, camData.position);

            // Set skybox if exists
            auto skybox = scene->getSkyboxCubeMap();
            if (skybox) {
                renderer.submitSkybox(skybox);
            }

            // Submit lights for this camera
            processLights(scene);

            // Submit renderables
            processRenderables(scene);

            // End camera rendering
            renderer.endCamera();
        }
    }

    void RenderSystem::processLights(Scene* scene)
    {
        PROFILE_SCOPE("RenderSystem::processLights");

        auto& registry = scene->getRegistry();
        auto& renderer = Renderer::get();

        // Submit point lights
        auto pointLightView = registry.view<LightComponent>();
        for (auto entity : pointLightView)
        {
            Entity ent(entity, scene);
            auto& light = registry.get<LightComponent>(entity);
            glm::vec3 position = ent.getPosition();

            submitPointLight(position, light.getColor(), light.getIntensity(),
                           light.getRadius(), light.getFallOff());
        }

        // Submit directional light (sun)
        auto sun = scene->getSun();
        if (sun && sun->getCastShadows())
        {
            renderer.submitSun(sun);
        }
    }

    void RenderSystem::processRenderables(Scene* scene)
    {
        PROFILE_SCOPE("RenderSystem::processRenderables");

        auto& registry = scene->getRegistry();
        auto view = registry.view<RendererComponent>();

        for (auto entity : view)
        {
            Entity ent(entity, scene);
            auto& rendererComp = registry.get<RendererComponent>(entity);

            glm::mat4 worldMatrix = ent.getWorldMatrix();
            auto material = rendererComp.getMaterial();
            if (!material) continue;

            // Submit single mesh
            if (rendererComp.hasMesh())
            {
                auto mesh = rendererComp.getMesh();
                if (mesh) {
                    submitMesh(mesh, material, worldMatrix);
                }
            }
            // Submit entire model
            else if (rendererComp.hasModel())
            {
                auto model = rendererComp.getModel();
                if (model) {
                    submitModel(model, material, worldMatrix);
                }
            }
        }
    }

    void RenderSystem::submitMesh(const std::shared_ptr<Mesh>& mesh,
                                  const std::shared_ptr<Material>& material,
                                  const glm::mat4& transform)
    {
        Renderer::get().submitMesh(mesh, material, transform);
    }

    void RenderSystem::submitModel(const std::shared_ptr<Model>& model,
                                   const std::shared_ptr<Material>& material,
                                   const glm::mat4& transform)
    {
        Renderer::get().submitModel(model, material, transform);
    }

    void RenderSystem::submitPointLight(const glm::vec3& position,
                                       const glm::vec3& color,
                                       float intensity,
                                       float radius,
                                       float fallOff)
    {
        Renderer::get().submitPointLight(position, color, intensity, radius, fallOff);
    }

} // namespace StrikeEngine
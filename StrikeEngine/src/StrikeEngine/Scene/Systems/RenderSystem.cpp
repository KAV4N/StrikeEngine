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

namespace StrikeEngine {

    RenderSystem::RenderSystem() {}

    void RenderSystem::onUpdate(float dt) {
        Scene* scene = World::get().getScene();
        if (!scene) return;

        processScene(scene);
    }

    void RenderSystem::onRender() {
        Scene* scene = World::get().getScene();
        if (!scene) return;
        
        Renderer::get().display();

        auto& renderer = Renderer::get();
        uint32_t screenWidth = renderer.getWidth();
        uint32_t screenHeight = renderer.getHeight();

        auto& registry = scene->getRegistry();
        auto view = registry.view<TextComponent>();
        
        for (auto entity : view) {
            auto& textComp = view.get<TextComponent>(entity);
            Entity ent(entity, scene);
            if (!textComp.isActive() || !ent.isActive()) continue;

            // Get normalized position (0-1) from TextComponent
            glm::vec2 normalizedPos = textComp.getPosition();
            
            // Convert to screen coordinates
            float screenX = normalizedPos.x * static_cast<float>(screenWidth);
            float screenY = normalizedPos.y * static_cast<float>(screenHeight);

            // Get scale from entity (use x scale as font scale)
            glm::vec3 scale = ent.getScale();

            FontRenderer::get().renderText(
                textComp.getText(),
                screenX,
                screenY,
                scale.x,
                textComp.getColor(),
                textComp.getPivot()
            );
        }
    }

    void RenderSystem::resize(uint32_t width, uint32_t height) {

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
        auto& registry = scene->getRegistry();
        auto& renderer = Renderer::get();
        std::multimap<int, CameraRenderData> cameras;

        auto skybox = scene->getSkybox(); 
        auto view = registry.view<CameraComponent>();
        
        for (auto entity : view) {
            Entity ent(entity, scene);
            if (!ent.isActive()) continue;

            auto& camera = registry.get<CameraComponent>(entity);

            CameraRenderData cameraRenderData;
            cameraRenderData.camera = camera;
            cameraRenderData.cameraPosition = ent.getPosition();
            cameraRenderData.cameraForward = ent.getForward();

            cameras.emplace(camera.getRenderOrder(), cameraRenderData);
        }

        for (const auto& [renderOrder, camData] : cameras) {
            renderer.beginCamera(camData.camera, camData.cameraPosition);

            if (skybox && skybox->isReady()) renderer.submitSkybox(skybox); 
            processLights(scene);

            auto& sun = scene->getSun();
            renderer.submitSun(&sun, sun.calculateLightSpaceMatrix(camData.camera));

            processRenderables(scene, camData.camera);

            renderer.endCamera();
        }
    }

    void RenderSystem::processLights(Scene* scene) {

        auto& registry = scene->getRegistry();
        auto& renderer = Renderer::get();


  

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

    void RenderSystem::processRenderables(Scene* scene, const CameraComponent& camera) {

        auto& registry = scene->getRegistry();
        auto view = registry.view<RendererComponent>();
        
        const auto& frustum = camera.getFrustum();

        for (auto entity : view) {
            Entity ent(entity, scene);
            auto& rendererComp = registry.get<RendererComponent>(entity);

            if (!ent.isActive() || !rendererComp.isActive()) continue;

            glm::mat4 worldMatrix = ent.getWorldMatrix();
            
            // Fetch assets on-demand
            auto texture = rendererComp.getTexture();
            auto model = rendererComp.getModel();
            glm::uvec3 color = rendererComp.getColor();

            bool readyModel = rendererComp.hasModel() && model && model->isReady();
            bool readyTexture = (texture == nullptr) || texture->isReady();

            if (!readyModel || !readyTexture) {
                continue;
            }
            
            // Frustum culling check
            const Bounds* bounds = nullptr;
            if (rendererComp.hasMesh()) {
                auto mesh = rendererComp.getMesh();
                if (mesh) {
                    bounds = &mesh->getBounds();
                }
            } else if (model) {
                bounds = &model->getBounds();
            }
            
            if (bounds) {
                // Transform bounds to world space
                glm::vec3 center = glm::vec3(worldMatrix * glm::vec4(bounds->getMidPoint(), 1.0f));
                glm::vec3 extents = (bounds->aabbMax - bounds->aabbMin) * 0.5f;
                
                // Extract scale from world matrix
                glm::vec3 scale = glm::vec3(
                    glm::length(glm::vec3(worldMatrix[0])),
                    glm::length(glm::vec3(worldMatrix[1])),
                    glm::length(glm::vec3(worldMatrix[2]))
                );
                
                glm::vec3 worldExtents = extents * scale;
                
                // Perform frustum culling
                if (!isInFrustum(frustum, center, worldExtents)) {
                    continue; // Skip rendering this object
                }
            }
            
            // Object is visible, submit for rendering
            if (rendererComp.hasMesh()) {
                auto mesh = rendererComp.getMesh();
                if (mesh) {
                    submitMesh(mesh, texture, color, worldMatrix);
                }
            } else if (rendererComp.hasModel()) {
                if (model) {
                    submitModel(model, texture, color, worldMatrix);
                }
            }
        }
    }

    bool RenderSystem::isInFrustum(const CameraComponent::Frustum& frustum, 
                                   const glm::vec3& center, 
                                   const glm::vec3& halfExtents) const {
        // Test against all 6 frustum planes
        for (int i = 0; i < 6; i++) {
            const glm::vec4& plane = frustum.planes[i];
            glm::vec3 normal = glm::vec3(plane);
            float distance = plane.w;
            
            // Find the "positive vertex" - the corner of the AABB that's furthest
            // along the plane normal direction
            glm::vec3 positiveVertex = center;
            positiveVertex.x += (normal.x >= 0.0f) ? halfExtents.x : -halfExtents.x;
            positiveVertex.y += (normal.y >= 0.0f) ? halfExtents.y : -halfExtents.y;
            positiveVertex.z += (normal.z >= 0.0f) ? halfExtents.z : -halfExtents.z;
            
            // Check if the positive vertex is outside this plane
            if (glm::dot(normal, positiveVertex) + distance < 0.0f) {
                return false; // Outside this plane, so outside frustum
            }
        }
        
        return true; // Inside all planes, so inside frustum
    }

    bool RenderSystem::isSphereInFrustum(const CameraComponent::Frustum& frustum,
                                        const glm::vec3& center,
                                        float radius) const {
        // Test against all 6 frustum planes
        for (int i = 0; i < 6; i++) {
            const glm::vec4& plane = frustum.planes[i];
            glm::vec3 normal = glm::vec3(plane);
            float distance = plane.w;
            
            // Calculate signed distance from sphere center to plane
            float dist = glm::dot(normal, center) + distance;
            
            // If distance is less than negative radius, sphere is completely outside
            if (dist < -radius) {
                return false;
            }
        }
        
        return true; // Inside or intersecting frustum
    }

    void RenderSystem::submitMesh(const std::shared_ptr<Mesh>& mesh,
                                  const std::shared_ptr<Texture>& texture,
                                  const glm::uvec3& color,
                                  const glm::mat4& transform) {
        Renderer::get().submitMesh(mesh, texture, color, transform);
    }

    void RenderSystem::submitModel(const std::shared_ptr<Model>& model,
                                   const std::shared_ptr<Texture>& texture,
                                   const glm::uvec3& color,
                                   const glm::mat4& transform) {
        Renderer::get().submitModel(model, texture, color, transform);
    }

    void RenderSystem::submitPointLight(const glm::vec3& position,
                                       const glm::uvec3& color,
                                       float intensity,
                                       float radius
                                       ) {
        Renderer::get().submitPointLight(position, color, intensity, radius);
    }
}
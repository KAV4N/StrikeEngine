#include "RenderSystem.h"
#include "StrikeEngine/Graphics/Renderer/Renderer.h"
#include <algorithm>
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/World.h"

namespace StrikeEngine {

    RenderSystem::RenderSystem() {}

    void RenderSystem::onUpdate(float dt) {
        Scene* scene = World::get().getCurrentScene();
        if (!scene) return;
        collectCameras(scene);
        collectLights(scene);
        collectRenderables(scene);
        cullRenderables();
        sortRenderables();
        submit();
    }

    void RenderSystem::submit() {
        auto& renderer = Renderer::get();
        for (const auto& cameraData : mCameraRenderData) {
            renderer.submit(cameraData);
        }
    }

    void RenderSystem::collectCameras(Scene* scene) {
        if (!scene) return;
        mCameraRenderData.clear();
        auto sceneGraph = scene->getSceneGraph();
        auto entities = sceneGraph->getEntitiesWithComponent<CameraComponent>();
        for (auto entity : entities) {
            if (!entity.isActive()) continue;
            auto& camera = entity.getComponent<CameraComponent>();
            glm::mat4 worldMatrix = entity.getWorldMatrix();
            // Extract position from world matrix (translation component)
            glm::vec3 cameraPosition = glm::vec3(worldMatrix[3]);
            CameraRenderData data{
                camera,
                cameraPosition, // Store camera position
                {},
                {},
                {},
                {}
            };
            mCameraRenderData.push_back(data);
        }

        std::sort(mCameraRenderData.begin(), mCameraRenderData.end(),
            [](const CameraRenderData& a, const CameraRenderData& b) {
                return a.camera.getRenderOrder() < b.camera.getRenderOrder();
            });
    }

    void RenderSystem::collectLights(Scene* scene) {
        if (!scene) return;
        auto sceneGraph = scene->getSceneGraph();

        // Collect point lights
        auto pointLightEntities = sceneGraph->getEntitiesWithComponent<PointLightComponent>();
        for (auto entity : pointLightEntities) {
            if (!entity.isActive()) continue;
            auto& light = entity.getComponent<PointLightComponent>();
            glm::vec3 position = entity.getPosition();
            
            PointLightData data;
            data.position = glm::vec4(position, 0.0f);  // w unused
            data.color = glm::vec4(light.getColor(), light.getIntensity());  // rgb: color, w: intensity
            data.attenuation = glm::vec4(light.getAttenuation(), light.getRadius());  // xyz: attenuation, w: radius
            
            for (auto& cameraData : mCameraRenderData) {
                cameraData.pointLights.push_back(data);
            }
        }

        // Collect directional lights
        auto dirLightEntities = sceneGraph->getEntitiesWithComponent<DirectionalLightComponent>();
        for (auto entity : dirLightEntities) {
            if (!entity.isActive()) continue;
            auto& light = entity.getComponent<DirectionalLightComponent>();
            
            DirectionalLightData data;
            data.direction = glm::vec4(light.getDirection(), 0.0f);  // w unused
            data.color = glm::vec4(light.getColor(), light.getIntensity());  // rgb: color, w: intensity
            
            for (auto& cameraData : mCameraRenderData) {
                cameraData.directionalLights.push_back(data);
            }
        }

        // Collect spot lights
        auto spotLightEntities = sceneGraph->getEntitiesWithComponent<SpotLightComponent>();
        for (auto entity : spotLightEntities) {
            if (!entity.isActive()) continue;
            auto& light = entity.getComponent<SpotLightComponent>();
            glm::vec3 position = entity.getPosition();
            
            SpotLightData data;
            data.position = glm::vec4(position, 0.0f);  // w unused
            data.direction = glm::vec4(light.getDirection(), 0.0f);  // w unused
            data.color = glm::vec4(light.getColor(), light.getIntensity());  // rgb: color, w: intensity
            data.anglesRadius = glm::vec4(light.getRadius(), light.getInnerConeAngle(), light.getOuterConeAngle(), 0.0f);  // x: radius, y: innerCone, z: outerCone, w: unused
            
            for (auto& cameraData : mCameraRenderData) {
                cameraData.spotLights.push_back(data);
            }
        }
    }

    void RenderSystem::collectRenderables(Scene* scene) {
        if (!scene) return;
        auto sceneGraph = scene->getSceneGraph();
        auto entities = sceneGraph->getEntitiesWithComponent<RendererComponent>();
        for (auto entity : entities) {
            if (!entity.isActive()) continue;

            auto& renderer = entity.getComponent<RendererComponent>();
            if (!renderer.hasMesh()) continue;

            auto mesh = renderer.getMesh();
            if (!mesh || !mesh->isReady()) continue;

            const auto& subMeshes = mesh->getSubMeshes();
            for (uint32_t i = 0; i < subMeshes.size(); ++i) {
                const auto& subMesh = subMeshes[i];
                auto material = renderer.getMaterialAsset(subMesh.slot);
                if (!material || !material->isReady()) continue;

                RenderItem item{
                    mesh,
                    material,
                    i,
                    entity.getWorldMatrix()
                };

                for (auto& cameraData : mCameraRenderData) {
                    cameraData.renderItems.push_back(item);
                }
            }
        }
    }

    void RenderSystem::cullRenderables() {
        for (auto& cameraData : mCameraRenderData) {
            auto& renderItems = cameraData.renderItems;
            renderItems.erase(
                std::remove_if(renderItems.begin(), renderItems.end(),
                    [&](const RenderItem& item) {
                        auto mesh = item.mesh;
                        const auto& subMesh = mesh->getSubMeshes()[item.subMeshIndex];
                        Bounds bounds = subMesh.bounds;
                        glm::mat4 worldMatrix = item.worldMatrix;
                        glm::vec3 corners[8];
                        boundsToCorners(bounds, worldMatrix, corners);
                        return !isAABBInFrustum(cameraData.camera.getFrustum(), corners);
                    }),
                renderItems.end()
            );
        }
    }

    void RenderSystem::sortRenderables() {
        for (auto& cameraData : mCameraRenderData) {
            std::sort(cameraData.renderItems.begin(), cameraData.renderItems.end(),
                [](const RenderItem& a, const RenderItem& b) {
                    GLuint shaderA = a.material->getShader()->getID();
                    GLuint shaderB = b.material->getShader()->getID();
                    return shaderA < shaderB;
                });
        }
    }

    void RenderSystem::boundsToCorners(const Bounds& bounds, const glm::mat4& worldMatrix, glm::vec3* corners) {
        glm::vec3 min = bounds.aabbMin;
        glm::vec3 max = bounds.aabbMax;
        corners[0] = glm::vec3(worldMatrix * glm::vec4(min.x, min.y, min.z, 1.0f));
        corners[1] = glm::vec3(worldMatrix * glm::vec4(max.x, min.y, min.z, 1.0f));
        corners[2] = glm::vec3(worldMatrix * glm::vec4(min.x, max.y, min.z, 1.0f));
        corners[3] = glm::vec3(worldMatrix * glm::vec4(max.x, max.y, min.z, 1.0f));
        corners[4] = glm::vec3(worldMatrix * glm::vec4(min.x, min.y, max.z, 1.0f));
        corners[5] = glm::vec3(worldMatrix * glm::vec4(max.x, min.y, max.z, 1.0f));
        corners[6] = glm::vec3(worldMatrix * glm::vec4(min.x, max.y, max.z, 1.0f));
        corners[7] = glm::vec3(worldMatrix * glm::vec4(max.x, max.y, max.z, 1.0f));
    }

    bool RenderSystem::isAABBInFrustum(const CameraComponent::Frustum& frustum, const glm::vec3* corners) {
        for (const auto& plane : frustum.planes) {
            bool outside = true;
            for (int i = 0; i < 8; ++i) {
                if (glm::dot(plane, glm::vec4(corners[i], 1.0f)) >= 0) {
                    outside = false;
                    break;
                }
            }
            if (outside) return false;
        }
        return true;
    }
}
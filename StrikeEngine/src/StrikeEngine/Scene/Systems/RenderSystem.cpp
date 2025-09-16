#include "RenderSystem.h"
#include "StrikeEngine/Graphics/Renderer/Renderer.h"
#include <algorithm>
#include "StrikeEngine/Scene/Entity.h"

namespace StrikeEngine {

    RenderSystem::RenderSystem() {}

    void RenderSystem::onUpdate(Scene* scene) {
        if (!scene) return;
        collectCameras(scene);
        collectRenderables(scene);
        //cullRenderables();
        sortRenderables();
        submit();
    }

    void RenderSystem::submit() {
        
        auto& renderer = Renderer::get();
        
        // Iterate through sorted cameras and submit to renderer
        for (const auto& cameraData : mCameraRenderData) {
            renderer.submit(cameraData);
        }
        
    }

    void RenderSystem::collectCameras(Scene* scene) {
        if (!scene) return; // Safety check
        mCameraRenderData.clear();
        auto view = scene->getRegistry().view<CameraComponent>();
        for (auto entityHandle : view) {
            Entity entity(*scene, entityHandle); // Pass scene by reference
            auto& camera = view.get<CameraComponent>(entityHandle);
            if (!entity.isActive()) continue;

            glm::vec3 position = entity.getWorldPosition();
            glm::quat rotation = entity.getWorldRotation();
            CameraRenderData data{
                &camera,
                camera.getViewProjectionMatrix(position, rotation),
                camera.calculateFrustum(position, rotation),
                {}
            };
            mCameraRenderData.push_back(data);
        }

        // Sort cameras by render order
        std::sort(mCameraRenderData.begin(), mCameraRenderData.end(),
            [](const CameraRenderData& a, const CameraRenderData& b) {
                return a.camera->getRenderOrder() < b.camera->getRenderOrder();
            });
    }

    void RenderSystem::collectRenderables(Scene* scene) {
        if (!scene) return; // Safety check
        auto view = scene->getRegistry().view<RendererComponent>();
        for (auto entityHandle : view) {
            Entity entity(*scene, entityHandle); // Pass scene by reference
            if (!entity.isActive()) continue;

            auto& renderer = view.get<RendererComponent>(entityHandle);
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
                        // Transform AABB to world space
                        glm::mat4 worldMatrix = item.worldMatrix;
                        glm::vec3 corners[8];
                        boundsToCorners(bounds, worldMatrix, corners);
                        return !isAABBInFrustum(cameraData.frustum, corners);
                    }),
                renderItems.end()
            );
        }
    }

    void RenderSystem::sortRenderables() {
        for (auto& cameraData : mCameraRenderData) {
            std::sort(cameraData.renderItems.begin(), cameraData.renderItems.end(),
                [](const RenderItem& a, const RenderItem& b) {
                    // Sort by shader ID to minimize shader switches
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
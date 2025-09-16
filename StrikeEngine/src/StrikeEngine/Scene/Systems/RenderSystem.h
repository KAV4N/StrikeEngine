#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <vector>
#include "StrikeEngine/Scene/Scene.h"
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include "StrikeEngine/Scene/Components/RendererComponent.h"
#include "StrikeEngine/Asset/Types/Mesh.h"
#include "StrikeEngine/Asset/Types/Material.h"

namespace StrikeEngine {

    struct RenderItem {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
        uint32_t subMeshIndex;
        glm::mat4 worldMatrix;
    };

    struct CameraRenderData {
        CameraComponent* camera;
        glm::mat4 viewProjectionMatrix;
        CameraComponent::Frustum frustum;
        std::vector<RenderItem> renderItems;
    };

    class RenderSystem {
    public:
        RenderSystem();

        void onUpdate(Scene* scene);

    private:
        void collectCameras(Scene* scene);
        void collectRenderables(Scene* scene);
        void cullRenderables();
        void sortRenderables();
        void submit();

        void boundsToCorners(const Bounds& bounds, const glm::mat4& worldMatrix, glm::vec3* corners);

        bool isAABBInFrustum(const CameraComponent::Frustum& frustum, const glm::vec3* corners);

    private:
        std::vector<CameraRenderData> mCameraRenderData;
    };

}
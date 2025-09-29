#pragma once
#include "SystemECS.h"
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <vector>
#include "StrikeEngine/Scene/Scene.h"
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include "StrikeEngine/Scene/Components/RendererComponent.h"
#include "StrikeEngine/Scene/Components/LightComponents.h"
#include "StrikeEngine/Asset/Types/Mesh.h"
#include "StrikeEngine/Asset/Types/Material.h"

namespace StrikeEngine {

    struct RenderItem {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
        uint32_t subMeshIndex;
        glm::mat4 worldMatrix;
    };

    struct alignas(16) PointLightData {
        glm::vec4 position;     // xyz: position, w: unused
        glm::vec4 color;        // rgb: color, w: intensity
        glm::vec4 attenuation;  // xyz: attenuation, w: radius
    };

    struct alignas(16) DirectionalLightData {
        glm::vec4 direction;    // xyz: direction, w: unused
        glm::vec4 color;        // rgb: color, w: intensity
    };

    struct alignas(16) SpotLightData {
        glm::vec4 position;         // xyz: position, w: unused
        glm::vec4 direction;        // xyz: direction, w: unused
        glm::vec4 color;            // rgb: color, w: intensity
        glm::vec4 anglesRadius;     // x: radius, y: innerCone, z: outerCone, w: unused
    };

    struct CameraRenderData {
        CameraComponent camera;
        glm::vec3 cameraPosition; // Added to store camera position
        std::vector<RenderItem> renderItems;
        std::vector<PointLightData> pointLights;
        std::vector<DirectionalLightData> directionalLights;
        std::vector<SpotLightData> spotLights;
    };

    class RenderSystem : public SystemECS {
    public:
        RenderSystem();

        void onUpdate(float dt) override;

    private:
        void collectCameras(Scene* scene);
        void collectRenderables(Scene* scene);
        void collectLights(Scene* scene);
        void cullRenderables();
        void sortRenderables();
        void submit();

        void boundsToCorners(const Bounds& bounds, const glm::mat4& worldMatrix, glm::vec3* corners);

        bool isAABBInFrustum(const CameraComponent::Frustum& frustum, const glm::vec3* corners);

    private:
        std::vector<CameraRenderData> mCameraRenderData;
    };
}
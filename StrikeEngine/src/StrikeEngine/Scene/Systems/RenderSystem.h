#pragma once
#include "SystemECS.h"
#include "StrikeEngine/Graphics/RenderData.h"
#include "StrikeEngine/Scene/Scene.h"

#include <entt/entt.hpp>
#include <vector>
#include <memory>

namespace StrikeEngine {

    class Model;
    class Texture;
    class Mesh;
    class CameraComponent;

    class RenderSystem : public SystemECS {
    public:
        RenderSystem();

        void onUpdate(float dt) override;
        void onRender() override;
        void resize(uint32_t width, uint32_t height);

        void submitMesh(const std::shared_ptr<Mesh>& mesh,
                       const std::shared_ptr<Texture>& texture,
                       const glm::uvec3& color,
                       const glm::mat4& transform);

        void submitModel(const std::shared_ptr<Model>& model,
                        const std::shared_ptr<Texture>& texture,
                        const glm::uvec3& color,
                        const glm::mat4& transform);

        void submitPointLight(const glm::vec3& position,
                            const glm::uvec3& color,
                            float intensity,
                            float radius
                            );

    private:
        void processScene(Scene* scene);
        void processRenderables(Scene* scene, const CameraComponent& camera);
        void processLights(Scene* scene);
        
        // Frustum culling helper functions
        bool isInFrustum(const CameraComponent::Frustum& frustum, 
                        const glm::vec3& center, 
                        const glm::vec3& halfExtents) const;
        
        bool isSphereInFrustum(const CameraComponent::Frustum& frustum,
                              const glm::vec3& center,
                              float radius) const;
    };
}
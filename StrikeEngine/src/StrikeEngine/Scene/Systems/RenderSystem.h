#pragma once
#include "SystemECS.h"
#include "StrikeEngine/Graphics/RenderData.h"
#include "StrikeEngine/Scene/Scene.h"

#include <entt/entt.hpp>
#include <vector>
#include <memory>

namespace Strike {

    class Model;
    class Texture;
    class Mesh;
    class CameraComponent;

    /**
     * @brief ECS system responsible for all scene rendering.
     *
     * RenderSystem collects visible renderables, lights, and cameras
     * from the active scene and submits them to the Renderer.
     * It supports multi-camera rendering, frustum culling, skyboxes,
     * lighting, and text rendering.
     */
    class RenderSystem : public SystemECS {
    public:
        /**
         * @brief Constructs the render system.
         */
        RenderSystem();

        /**
         * @brief Updates render-related scene data.
         *
         * Gathers cameras, lights, and renderable objects
         * from the active scene and prepares them for rendering.
         *
         * @param dt Delta time (in seconds) since the last frame.
         */
        void onUpdate(float dt) override;

        /**
         * @brief Executes the final rendering pass.
         *
         * Displays the rendered frame and renders all screen-space
         * elements such as text components.
         */
        void onRender() override;

        /**
         * @brief Resizes render targets and updates camera projections.
         *
         * @param width New viewport width.
         * @param height New viewport height.
         */
        void resize(uint32_t width, uint32_t height);

        /**
         * @brief Submits a single mesh for rendering.
         *
         * @param mesh Mesh to render.
         * @param texture Optional texture applied to the mesh.
         * @param color Vertex or material color tint.
         * @param transform World transform matrix.
         */
        void submitMesh(const std::shared_ptr<Mesh>& mesh,
                        const std::shared_ptr<Texture>& texture,
                        const glm::uvec3& color,
                        const glm::mat4& transform);

        /**
         * @brief Submits a model (multi-mesh asset) for rendering.
         *
         * @param model Model asset to render.
         * @param texture Optional override texture.
         * @param color Color tint applied to the model.
         * @param transform World transform matrix.
         */
        void submitModel(const std::shared_ptr<Model>& model,
                         const std::shared_ptr<Texture>& texture,
                         const glm::uvec3& color,
                         const glm::mat4& transform);

        /**
         * @brief Submits a point light to the renderer.
         *
         * @param position World-space position of the light.
         * @param color Light color.
         * @param intensity Light intensity.
         * @param radius Effective light radius.
         */
        void submitPointLight(const glm::vec3& position,
                              const glm::uvec3& color,
                              float intensity,
                              float radius);

    private:
        /**
         * @brief Processes all cameras and render passes in the scene.
         *
         * Cameras are sorted by render order and rendered sequentially.
         *
         * @param scene Active scene.
         */
        void processScene(Scene* scene);

        /**
         * @brief Submits visible renderable entities to the renderer.
         *
         * Performs frustum culling before submitting meshes or models.
         *
         * @param scene Active scene.
         * @param camera Camera used for visibility checks.
         */
        void processRenderables(Scene* scene, const CameraComponent& camera);

        /**
         * @brief Collects and submits all active lights in the scene.
         *
         * @param scene Active scene.
         */
        void processLights(Scene* scene);

        /**
         * @brief Tests whether an AABB is inside the camera frustum.
         *
         * @param frustum Camera frustum planes.
         * @param center World-space center of the bounding box.
         * @param halfExtents Half-size extents of the bounding box.
         * @return True if the box is inside or intersects the frustum.
         */
        bool isInFrustum(const CameraComponent::Frustum& frustum,
                         const glm::vec3& center,
                         const glm::vec3& halfExtents) const;

        /**
         * @brief Tests whether a bounding sphere is inside the camera frustum.
         *
         * @param frustum Camera frustum planes.
         * @param center World-space center of the sphere.
         * @param radius Sphere radius.
         * @return True if the sphere is inside or intersects the frustum.
         */
        bool isSphereInFrustum(const CameraComponent::Frustum& frustum,
                               const glm::vec3& center,
                               float radius) const;
    };
}

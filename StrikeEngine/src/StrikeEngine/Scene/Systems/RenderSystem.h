#pragma once
#include "SystemECS.h"
#include "StrikeEngine/Graphics/RenderCommands.h"
#include "StrikeEngine/Scene/Scene.h"

#include <entt/entt.hpp>
#include <vector>
#include <memory>

namespace StrikeEngine {

    class Model;
    class Material;
    class Mesh;

    class RenderSystem : public SystemECS {
    public:
        RenderSystem();

        void onUpdate(float dt) override;
        void onRender() override;
        void resize(uint32_t width, uint32_t height);

        // Submission API
        void submitMesh(const std::shared_ptr<Mesh>& mesh,
                       const std::shared_ptr<Material>& material,
                       const glm::mat4& transform);

        void submitModel(const std::shared_ptr<Model>& model,
                        const std::shared_ptr<Material>& material,
                        const glm::mat4& transform);

        void submitPointLight(const glm::vec3& position,
                            const glm::vec3& color,
                            float intensity,
                            float radius,
                            float fallOff);

    private:
        void processScene(Scene* scene);
        void processRenderables(Scene* scene);
        void processLights(Scene* scene);
    };

} // namespace StrikeEngine
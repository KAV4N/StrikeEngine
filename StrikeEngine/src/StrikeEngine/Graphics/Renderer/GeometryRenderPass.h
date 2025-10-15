#pragma once
#include "RenderPass.h"
#include <glad/glad.h>
#include "StrikeEngine/Scene/Systems/RenderSystem.h" // For CameraRenderData and light data structs
#include "StrikeEngine/Asset/Types/Shader.h" // For Shader class

namespace StrikeEngine {

    class Renderer;

    class GeometryRenderPass : public RenderPass {
    public:
        GeometryRenderPass(Renderer& renderer, const std::string& name = "GeometryPass");
        ~GeometryRenderPass() override;

        void execute(const CameraRenderData& cameraData) override;
        void setup() override;
        void cleanup() override;

    private:
        void setupOpenGLState();
        void restoreOpenGLState();
        void bindPointLights(const std::vector<PointLightData>& pointLights, std::shared_ptr<Shader> shader);
        void bindDirectionalLights(const std::vector<DirectionalLightData>& directionalLights, std::shared_ptr<Shader> shader);
        void bindSpotLights(const std::vector<SpotLightData>& spotLights, std::shared_ptr<Shader> shader);

        Renderer& mRenderer;
    };
}
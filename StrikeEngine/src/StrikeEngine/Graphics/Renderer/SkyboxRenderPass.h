#pragma once
#include "RenderPass.h"
#include "Renderer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "StrikeEngine/Graphics/Skybox.h"

namespace StrikeEngine {

    class SkyboxRenderPass : public RenderPass {
    public:
        SkyboxRenderPass(Renderer& renderer, const std::string& name);
        ~SkyboxRenderPass() override;

        void setup() override;
        void cleanup() override;
        void execute(const CameraRenderData& cameraData) override;

    private:
        void setupOpenGLState();
        void restoreOpenGLState();

        Renderer& mRenderer;
        Skybox* mSkybox;
    };

}
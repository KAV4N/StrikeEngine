#pragma once
#include "RenderPass.h"
#include <glad/glad.h>

namespace StrikeEngine {

    class Renderer;

    class SkyboxRenderPass : public RenderPass {
    public:
        SkyboxRenderPass(Renderer& renderer, const std::string& name = "SkyboxPass");
        ~SkyboxRenderPass() override;

        void execute(const CameraRenderData& cameraData) override;
        void setup() override;
        void cleanup() override;

    private:
        void setupOpenGLState();
        void restoreOpenGLState();

        Renderer& mRenderer;
        GLint mPreviousDepthFunc;
    };

}
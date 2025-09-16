#pragma once
#include "RenderPass.h"
#include <glad/glad.h>

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

        Renderer& mRenderer;
    };

}
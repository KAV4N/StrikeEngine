#pragma once
#include "RenderPass.h"
#include <glm/glm.hpp>

namespace StrikeEngine {

    class Skybox;
    class Renderer;

    class SkyboxRenderPass : public RenderPass {
    public:
        SkyboxRenderPass(Renderer& renderer);
        ~SkyboxRenderPass() override;

        void setup() override;
        void cleanup() override;
        void execute(const CameraRenderData& cameraData) override;

    private:
        void setupOpenGLState();
        void restoreOpenGLState();

        Renderer& mRenderer;
        
        std::unique_ptr<Skybox> mSkybox;
    };

}
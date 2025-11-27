#pragma once
#include "RenderPass.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace StrikeEngine {
    class Shader;
    class Renderer;

    class LightCullingPass : public RenderPass {
    public:
        LightCullingPass(Renderer& renderer);
        ~LightCullingPass();

        void setup() override;
        void execute(const CameraRenderData& cameraData) override;
        void cleanup() override;

    private:
        void initSSBOs();

    private:
        Renderer& mRenderer;
        GLuint mLightSSBO;
    };

}
#pragma once
#include "RenderPass.h"
#include <glm/glm.hpp>
#include <memory>

namespace StrikeEngine {

    class FrameBuffer;
    class Renderer;

    class ShadowMapPass : public RenderPass {
    public:
        ShadowMapPass(Renderer& renderer);
        ~ShadowMapPass() override;

        void setup() override;
        void cleanup() override;
        void execute(const CameraRenderData& cameraData) override;

        GLuint getShadowMapTexture() const;

    private:
        void setupOpenGLState();
        void restoreOpenGLState(uint32_t width, uint32_t height);

    private:
        Renderer& mRenderer;
        std::unique_ptr<FrameBuffer> mShadowMap;
        uint32_t mShadowMapSize = 2048;
    };
}
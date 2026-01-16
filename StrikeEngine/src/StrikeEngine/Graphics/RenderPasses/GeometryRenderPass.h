#pragma once

#include "RenderPass.h"
#include <memory>
#include <vector>
#include <glad/glad.h>

namespace StrikeEngine {

    class Renderer;
    class Shader;

    class GeometryRenderPass : public RenderPass {
    public:
        GeometryRenderPass(Renderer& renderer);
        ~GeometryRenderPass() override;

        void setup() override;
        void cleanup() override;
        void execute(const CameraRenderData& cameraData) override;

    private:
        void setupOpenGLState();
        void restoreOpenGLState();
        void renderInstanceBatch(const InstanceBatch& batch,
                                const CameraRenderData& cameraData);
        void createWhiteTexture();
        void destroyWhiteTexture();

    private:
        Renderer& mRenderer;
        std::shared_ptr<Shader> mShader;
        GLuint mWhiteTextureID;
    };

}
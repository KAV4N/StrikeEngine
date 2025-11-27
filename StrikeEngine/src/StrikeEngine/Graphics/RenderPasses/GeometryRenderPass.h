#pragma once
#include "RenderPass.h"
#include <memory>
#include <vector>

namespace StrikeEngine {

    class Renderer;
    struct PointLightData;
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

    private:
        Renderer& mRenderer;
    };

}
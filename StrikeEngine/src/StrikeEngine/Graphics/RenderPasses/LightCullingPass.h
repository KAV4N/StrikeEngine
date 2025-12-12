#pragma once
#include "RenderPass.h"
#include "StrikeEngine/Graphics/Shader.h"

#include <glm/glm.hpp>
#include <memory>

namespace StrikeEngine {

class Renderer;

class LightCullingPass : public RenderPass {
public:
    LightCullingPass(Renderer& renderer);
    ~LightCullingPass();

    void setup() override;
    void execute(const CameraRenderData& cameraData) override;
    void cleanup() override;

private:
    void initClusterBuffers();
    void buildClusters(const CameraRenderData& cameraData);
    void cullLights(const CameraRenderData& cameraData);
public:
    // Cluster grid configuration (tweakable)
    static constexpr uint32_t CLUSTER_X = 16;
    static constexpr uint32_t CLUSTER_Y = 9;
    static constexpr uint32_t CLUSTER_Z = 24;

    // Total number of clusters
    static constexpr uint32_t TOTAL_CLUSTERS = CLUSTER_X * CLUSTER_Y * CLUSTER_Z;

    // Max lights per cluster (must match shader)
    static constexpr uint32_t MAX_LIGHTS_PER_CLUSTER = 100;
private:
    Renderer& mRenderer;

    // SSBOs
    GLuint mLightSSBO = 0;
    GLuint mClusterSSBO = 0;

    // Compute shaders
    std::shared_ptr<ComputeShader> mClusterBuildShader;
    std::shared_ptr<ComputeShader> mLightCullShader;


};

} // namespace StrikeEngine
#pragma once
#include "RenderPass.h"
#include "StrikeEngine/Graphics/Shader.h"

#include <glm/glm.hpp>
#include <memory>

namespace StrikeEngine {

class Renderer;

/**
 * @brief Compute shader-based light culling pass using clustered rendering
 * 
 * Divides view frustum into 3D clusters and assigns visible lights to each cluster
 * for efficient lighting calculations in the geometry pass.
 * 
 * @note Uses compute shaders for GPU-based culling
 */
class LightCullingPass : public RenderPass {
public:
    /**
     * @brief Construct a new LightCullingPass object
     * 
     * @param renderer Reference to the main renderer
     */
    LightCullingPass(Renderer& renderer);
    
    /**
     * @brief Destroy the LightCullingPass object
     */
    ~LightCullingPass();

    /**
     * @brief Setup light culling pass resources
     */
    void setup() override;
    
    /**
     * @brief Execute the light culling pass
     * 
     * Builds clusters and performs light culling using compute shaders
     * 
     * @param cameraData Camera rendering data for this frame
     */
    void execute(const CameraRenderData& cameraData) override;
    
    /**
     * @brief Clean up light culling pass resources
     */
    void cleanup() override;

public:
    // Cluster grid configuration (tweakable)
    static constexpr uint32_t CLUSTER_X = 16; /**< Number of clusters in X dimension (screen width) */
    static constexpr uint32_t CLUSTER_Y = 9;  /**< Number of clusters in Y dimension (screen height) */
    static constexpr uint32_t CLUSTER_Z = 24; /**< Number of clusters in Z dimension (depth) */

    // Total number of clusters
    static constexpr uint32_t TOTAL_CLUSTERS = CLUSTER_X * CLUSTER_Y * CLUSTER_Z;

    // Max lights per cluster (must match shader)
    static constexpr uint32_t MAX_LIGHTS_PER_CLUSTER = 100; /**< Maximum number of lights assigned to a single cluster */

private:
    void initClusterBuffers();
    void buildClusters(const CameraRenderData& cameraData);
    void cullLights(const CameraRenderData& cameraData);

private:
    Renderer& mRenderer; /**< Reference to main renderer */

    // SSBOs
    GLuint mLightSSBO = 0;     /**< Shader Storage Buffer Object for point lights */
    GLuint mClusterSSBO = 0;   /**< Shader Storage Buffer Object for cluster data */

    // Compute shaders
    std::shared_ptr<ComputeShader> mClusterBuildShader; /**< Compute shader for building cluster AABBs */
    std::shared_ptr<ComputeShader> mLightCullShader;    /**< Compute shader for culling lights per cluster */
};
}
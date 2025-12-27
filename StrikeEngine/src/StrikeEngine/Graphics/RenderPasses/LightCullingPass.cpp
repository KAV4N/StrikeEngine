#include "strikepch.h"
#include "LightCullingPass.h"
#include "StrikeEngine/Graphics/Renderer.h"
#include "StrikeEngine/Graphics/Shader.h"

namespace StrikeEngine {

LightCullingPass::LightCullingPass(Renderer& renderer)
    : RenderPass("LightCullingPass"), mRenderer(renderer)
{
}

LightCullingPass::~LightCullingPass()
{
    cleanup();
}

void LightCullingPass::setup()
{
    initClusterBuffers();

    // Load compute shaders (make sure these files are in GenShaders.h)
    mClusterBuildShader = ShaderManager::get().getComputeShader("clusterBuild.glsl");
    mLightCullShader     = ShaderManager::get().getComputeShader("cullLight.glsl");

    if (!mClusterBuildShader || !mLightCullShader)
    {
        throw std::runtime_error("Failed to load clustered rendering compute shaders!");
    }
}

void LightCullingPass::initClusterBuffers()
{
    // --- Point Light SSBO (binding = 2) ---
    glGenBuffers(1, &mLightSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mLightSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW); // resized later
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, mLightSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // --- Cluster AABB + Light Index List SSBO (binding = 1) ---
    struct ClusterData
    {
        glm::vec4 minPoint;
        glm::vec4 maxPoint;
        uint32_t count;
        uint32_t lightIndices[MAX_LIGHTS_PER_CLUSTER];
    };

    glGenBuffers(1, &mClusterSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mClusterSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 TOTAL_CLUSTERS * sizeof(ClusterData),
                 nullptr,
                 GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mClusterSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightCullingPass::execute(const CameraRenderData& cameraData)
{
    const auto& camera = cameraData.camera;

    // Update light SSBO
    if (!cameraData.pointLights.empty())
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mLightSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     cameraData.pointLights.size() * sizeof(PointLight),
                     cameraData.pointLights.data(),
                     GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    // --- Step 1: Build clusters in view space ---
    buildClusters(cameraData);

    // --- Step 2: Cull lights per cluster ---
    cullLights(cameraData);

    // Ensure all compute work is done
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void LightCullingPass::buildClusters(const CameraRenderData& cameraData)
{
    const auto& cam = cameraData.camera;
    const auto& viewport = cam.getViewportRect();
    

    float viewportWidth = viewport.width * mRenderer.getWidth();
    float viewportHeight = viewport.height * mRenderer.getHeight();
    
    const glm::mat4 invProj = glm::inverse(cam.getProjectionMatrix());
    const float zNear = cam.getNearPlane();
    const float zFar = cam.getFarPlane();
    
    mClusterBuildShader->bind();
    mClusterBuildShader->setFloat("zNear", zNear);
    mClusterBuildShader->setFloat("zFar", zFar);
    mClusterBuildShader->setMat4("inverseProjection", invProj);
    
    mClusterBuildShader->setVec2("screenDimensions", 
                                  glm::vec2(viewportWidth, viewportHeight));
    
    mClusterBuildShader->setInt("gridSize.x", CLUSTER_X);
    mClusterBuildShader->setInt("gridSize.y", CLUSTER_Y);
    mClusterBuildShader->setInt("gridSize.z", CLUSTER_Z);
    
    mClusterBuildShader->dispatch(CLUSTER_X, CLUSTER_Y, CLUSTER_Z);
    mClusterBuildShader->waitFinish();
}

void LightCullingPass::cullLights(const CameraRenderData& cameraData)
{
    const glm::mat4 viewMatrix = cameraData.camera.getViewMatrix();

    mLightCullShader->bind();
    mLightCullShader->setMat4("viewMatrix", viewMatrix);

    // Dispatch one workgroup per cluster
    mLightCullShader->dispatch(TOTAL_CLUSTERS / 128 + 1, 1, 1); // 128 threads per group
    mLightCullShader->waitFinish();
}

void LightCullingPass::cleanup()
{
    if (mLightSSBO)
    {
        glDeleteBuffers(1, &mLightSSBO);
        mLightSSBO = 0;
    }
    if (mClusterSSBO)
    {
        glDeleteBuffers(1, &mClusterSSBO);
        mClusterSSBO = 0;
    }
}

} // namespace StrikeEngine
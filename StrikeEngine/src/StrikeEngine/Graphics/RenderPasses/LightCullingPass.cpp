#include "LightCullingPass.h"
#include "StrikeEngine/Graphics/Renderer.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "StrikeEngine/Graphics/Shader.h"


namespace StrikeEngine {

    LightCullingPass::LightCullingPass(Renderer& renderer)
        : RenderPass("LightCullingPass"), mRenderer(renderer), mLightSSBO(0) {
    }

    LightCullingPass::~LightCullingPass() {
        cleanup();
    }

    void LightCullingPass::setup() {
        initSSBOs();
    }

    void LightCullingPass::initSSBOs() {
        // Light SSBO
        glGenBuffers(1, &mLightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mLightSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, mLightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void LightCullingPass::execute(const CameraRenderData& cameraData) {
        // Update light SSBO - upload all point lights
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mLightSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, 
                     cameraData.pointLights.size() * sizeof(PointLight), 
                     cameraData.pointLights.data(), 
                     GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void LightCullingPass::cleanup() {
        if (mLightSSBO) {
            glDeleteBuffers(1, &mLightSSBO);
            mLightSSBO = 0;
        }
    }

}
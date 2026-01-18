#include "strikepch.h"
#include "GeometryRenderPass.h"
#include "StrikeEngine/Graphics/Renderer.h"
#include "ShadowMapPass.h"
#include "LightCullingPass.h"
#include "StrikeEngine/Graphics/Shader.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace StrikeEngine {

    GeometryRenderPass::GeometryRenderPass(Renderer& renderer)
        : RenderPass("GeometryPass"), mRenderer(renderer), mWhiteTextureID(0) {
    }

    GeometryRenderPass::~GeometryRenderPass() {
        cleanup();
    }

    void GeometryRenderPass::createWhiteTexture() {
        // Create 1x1 white texture
        glGenTextures(1, &mWhiteTextureID);
        glBindTexture(GL_TEXTURE_2D, mWhiteTextureID);
        
        unsigned char whitePixel[] = { 255, 255, 255, 255 };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GeometryRenderPass::destroyWhiteTexture() {
        if (mWhiteTextureID != 0) {
            glDeleteTextures(1, &mWhiteTextureID);
            mWhiteTextureID = 0;
        }
    }

    void GeometryRenderPass::setup() {
        mShader = ShaderManager::get().getShader("geometry.glsl");
        createWhiteTexture();
    }

    void GeometryRenderPass::cleanup() {
        mShader.reset();
        destroyWhiteTexture();
    }

    void GeometryRenderPass::execute(const CameraRenderData& cameraData) {
        if (!mShader) return;

        setupOpenGLState();

        for (const auto& [key, batch] : cameraData.instanceBatches) {
            renderInstanceBatch(batch, cameraData);
        }

        restoreOpenGLState();
    }

    void GeometryRenderPass::renderInstanceBatch(const InstanceBatch& batch,
                                              const CameraRenderData& cameraData) {
        if (batch.worldMatrices.empty()) return;

        auto camera = cameraData.camera;
        glm::vec3 cameraPos = cameraData.cameraPosition;

        mShader->bind();

        // Set separate view and projection matrices instead of combined
        mShader->setMat4("uView", camera.getViewMatrix());
        mShader->setMat4("uProjection", camera.getProjectionMatrix());
        mShader->setVec3("uViewPos", cameraPos);

        // Set color with blend (RGB in 0-255 range converted to 0-1, A is blend in 0-1 range)
        mShader->setVec3("uColor", glm::vec3(batch.color) / 255.0f);

        // Always bind a texture (either actual texture or white texture)
        glActiveTexture(GL_TEXTURE0);
        if (batch.texture) {
            batch.texture->bind(0);
        } else {
            glBindTexture(GL_TEXTURE_2D, mWhiteTextureID);
        }
        mShader->setInt("uTexture", 0);

        // Clustered lighting uniforms
        auto lightPass = mRenderer.getPass<LightCullingPass>();
        mShader->setFloat("uZNear", camera.getNearPlane());
        mShader->setFloat("uZFar", camera.getFarPlane());
        mShader->setUVec3("uGridSize", glm::uvec3(
            lightPass->CLUSTER_X, 
            lightPass->CLUSTER_Y, 
            lightPass->CLUSTER_Z
        ));

        const auto& viewport = camera.getViewportRect();
        
        float viewportWidth = viewport.width * mRenderer.getWidth();
        float viewportHeight = viewport.height * mRenderer.getHeight();

        mShader->setUVec2("uScreenDimensions", glm::uvec2(
            viewportWidth, 
            viewportHeight
        ));

        // Sun
        Sun* sun = cameraData.sunData.sun;
        mShader->setVec3("uSun.direction", sun->getDirection());
        mShader->setVec3("uSun.color", sun->getColor() / 255.0f);
        mShader->setFloat("uSun.intensity", sun->getIntensity());
        mShader->setInt("uCastShadows", sun->getCastShadows() ? 1 : 0);

        // Shadows
        ShadowMapPass* shadowPass = mRenderer.getPass<ShadowMapPass>();
        if (shadowPass && sun->getCastShadows()) {
            mShader->setMat4("uLightSpaceMatrix", cameraData.sunData.lightSpaceMatrix);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, shadowPass->getShadowMapTexture());
            mShader->setInt("uShadowMap", 1);
        }

        // Render instanced
        GLuint vao = batch.mesh->getVAO();
        glBindVertexArray(vao);

        const auto& indices = batch.mesh->getIndices();
        size_t indiceCount = indices.size();
        size_t totalInstances = batch.worldMatrices.size();
        size_t offset = 0;

        while (offset < totalInstances) {
            size_t instanceCount = std::min(totalInstances - offset, (size_t)Renderer::MAX_INSTANCES);
            const glm::mat4* matrixPtr = batch.worldMatrices.data() + offset;
            batch.mesh->updateInstanceBuffer(matrixPtr, instanceCount);
            
            glDrawElementsInstanced(GL_TRIANGLES,
                                    static_cast<GLsizei>(indiceCount),
                                    GL_UNSIGNED_INT,
                                    0,
                                    static_cast<GLsizei>(instanceCount));
            
            offset += instanceCount;
        }

        glBindVertexArray(0);
        mShader->unbind();
    }

    void GeometryRenderPass::setupOpenGLState() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void GeometryRenderPass::restoreOpenGLState() {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
    }

}
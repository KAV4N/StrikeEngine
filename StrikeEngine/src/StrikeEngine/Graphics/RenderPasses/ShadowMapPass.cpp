#include "ShadowMapPass.h"
#include "StrikeEngine/Graphics/Renderer.h"
#include "StrikeEngine/Graphics/FrameBuffer.h"
#include "StrikeEngine/Graphics/Shader.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Material.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace StrikeEngine {

    ShadowMapPass::ShadowMapPass(Renderer& renderer)
        : RenderPass("SunShadowPass"), mRenderer(renderer) {
        mShadowMap = std::make_unique<FrameBuffer>(mShadowMapSize, mShadowMapSize);
    }

    ShadowMapPass::~ShadowMapPass() {
        cleanup();
    }

    void ShadowMapPass::setup() {
        mShadowMap->bind();
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        mShadowMap->unBind();
    }

    void ShadowMapPass::cleanup() {
        mShadowMap.reset();
    }

    void ShadowMapPass::execute(const CameraRenderData& cameraData) {
        if (!cameraData.sunData.castShadows) return;
        if (cameraData.sunData.shadowBatches.empty()) return;

        setupOpenGLState();

        // Bind shadow map FBO
        mShadowMap->bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        auto depthShader = ShaderManager::get().getShader("sunShadowMap.glsl");
        if (!depthShader) {
            mShadowMap->unBind();
            restoreOpenGLState(mRenderer.getWidth(), mRenderer.getHeight());
            return;
        }

        depthShader->bind();
        depthShader->setMat4("uLightSpaceMatrix", cameraData.sunData.lightSpaceMatrix);

        // Render all shadow-casting instance batches
        for (const auto& [key, batch] : cameraData.sunData.shadowBatches) {
            if (!batch.mesh || !batch.mesh->getVAO() || batch.worldMatrices.empty())
                continue;

            glBindVertexArray(batch.mesh->getVAO());
            const auto& indices = batch.mesh->getIndices();
            size_t indiceCount = indices.size();
            size_t totalInstances = batch.worldMatrices.size();
            size_t offset = 0;

            while (offset < totalInstances) {
                size_t instanceCount = std::min(totalInstances - offset, (size_t)Renderer::MAX_INSTANCES);
                const glm::mat4* matrixPtr = batch.worldMatrices.data() + offset;

                batch.mesh->updateInstanceBuffer(matrixPtr, instanceCount);

                glDrawElementsInstanced(
                    GL_TRIANGLES,
                    indiceCount,
                    GL_UNSIGNED_INT,
                    0, 
                    instanceCount);


                offset += instanceCount;
            }

            glBindVertexArray(0);
        }

        mShadowMap->unBind();

        restoreOpenGLState(mRenderer.getWidth(), mRenderer.getHeight());
    }

    void ShadowMapPass::setupOpenGLState() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);  // Front-face culling reduces peter-panning artifacts
        glViewport(0, 0, mShadowMapSize, mShadowMapSize);
    }

    void ShadowMapPass::restoreOpenGLState(uint32_t width, uint32_t height) {
        glCullFace(GL_BACK);
        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    GLuint ShadowMapPass::getShadowMapTexture() const {
        return mShadowMap->getDepthTextureID();
    }

} // namespace StrikeEngine
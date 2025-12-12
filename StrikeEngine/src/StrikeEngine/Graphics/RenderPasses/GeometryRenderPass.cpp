#include "GeometryRenderPass.h"
#include "StrikeEngine/Graphics/Renderer.h"
#include "ShadowMapPass.h"
#include "StrikeEngine/Graphics/Shader.h"
#include "StrikeEngine/Scene/Systems/RenderSystem.h"
#include "StrikeEngine/Scene/World.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Material.h"

#include "LightCullingPass.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace StrikeEngine {

    GeometryRenderPass::GeometryRenderPass(Renderer& renderer)
        : RenderPass("GeometryPass"), mRenderer(renderer) {
    }

    GeometryRenderPass::~GeometryRenderPass() {
        cleanup();
    }

    void GeometryRenderPass::setup() {
    }

    void GeometryRenderPass::cleanup() {
    }

    void GeometryRenderPass::execute(const CameraRenderData& cameraData) {
        if (!isEnabled()) return;

        setupOpenGLState();

        for (const auto& [key, batch] : cameraData.instanceBatches) {
            renderInstanceBatch(batch, cameraData);
        }

        restoreOpenGLState();
    }

    void GeometryRenderPass::renderInstanceBatch(const InstanceBatch& batch,
                                              const CameraRenderData& cameraData)
    {
        if (!batch.mesh || !batch.material || !batch.mesh->getVAO() || batch.worldMatrices.empty())
            return;

        auto camera = cameraData.camera;
        glm::vec3 cameraPos = cameraData.cameraPosition;

        ShadowMapPass* shadowPass = mRenderer.getPass<ShadowMapPass>();
        Scene* scene = World::get().getScene();
        auto sun = scene ? scene->getSun() : nullptr;

        batch.material->bind();
        auto shader = batch.material->getShader();

        shader->setMat4("uViewProjection", camera.getViewProjectionMatrix());
        shader->setVec3("uViewPos", cameraPos);

        // light clusters
        auto lightPass = mRenderer.getPass<LightCullingPass>();
        shader->setFloat("uNear", cameraData.camera.getNearPlane());
        shader->setFloat("uFar", cameraData.camera.getFarPlane());
        shader->setVec3("uGridSize", glm::vec3(lightPass->CLUSTER_X, lightPass->CLUSTER_Y, lightPass->CLUSTER_Z));
        shader->setVec2("uScreenDimensions", glm::vec2(mRenderer.getWidth(), mRenderer.getHeight()));

        if (sun) {
            shader->setVec3("uSun.direction", sun->getDirection());
            shader->setVec3("uSun.color", sun->getColor() / 255.0f);
            shader->setFloat("uSun.intensity", sun->getIntensity());
            shader->setInt("uCastShadows", cameraData.sunData.castShadows ? 1 : 0);

            if (shadowPass && cameraData.sunData.castShadows) {
                shader->setMat4("uLightSpaceMatrix", cameraData.sunData.lightSpaceMatrix);
                glActiveTexture(GL_TEXTURE0 + 1);
                glBindTexture(GL_TEXTURE_2D, shadowPass->getShadowMapTexture());
                shader->setInt("uShadowMap", 1);
            }
        }
        

        GLuint vao = batch.mesh->getVAO();
        glBindVertexArray(vao);

        const auto& indices = batch.mesh->getIndices();
        size_t indiceCount = indices.size();

            
        size_t totalInstances = batch.worldMatrices.size();
        size_t offset = 0;

        while (offset < totalInstances)
        {
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
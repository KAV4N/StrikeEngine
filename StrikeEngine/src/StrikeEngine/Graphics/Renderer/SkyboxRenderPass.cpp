// SkyboxRenderPass.cpp - Removed redundant viewport/scissor setup
#include "SkyboxRenderPass.h"
#include "StrikeEngine/Scene/World.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace StrikeEngine {

    SkyboxRenderPass::SkyboxRenderPass(Renderer& renderer, const std::string& name)
        : RenderPass(name), mRenderer(renderer), mSkybox(World::get().getSkybox()) {
    }

    SkyboxRenderPass::~SkyboxRenderPass() {
        cleanup();
    }

    void SkyboxRenderPass::setup() {

    }

    void SkyboxRenderPass::cleanup() {

    }

    void SkyboxRenderPass::execute(const CameraRenderData& cameraData) {
        if (!isEnabled() || !mSkybox || !mSkybox->isCubeMapValid() || !mSkybox->isShaderValid()) {
            return;
        }

        auto camera = cameraData.camera;

        setupOpenGLState();

        glm::mat4 view = camera.getViewMatrix();
        view = glm::mat4(glm::mat3(view)); 
        glm::mat4 projection = camera.getProjectionMatrix();
        glm::mat4 viewProjection = projection * view;

        auto shader = mSkybox->getShader();
        auto cubeMap = mSkybox->getCubeMap();

        shader->bind();
        shader->setMat4("uViewProjection", viewProjection);
        shader->setInt("uSkybox", 0);

        glBindVertexArray(mSkybox->getVAO());
        cubeMap->bind(0);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        cubeMap->unbind();
        shader->unbind();
        
        restoreOpenGLState();
    }

    void SkyboxRenderPass::setupOpenGLState() {
        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL); 
        glDepthMask(GL_FALSE);  


        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT); 
        glFrontFace(GL_CCW);
    }

    void SkyboxRenderPass::restoreOpenGLState() {
        glDepthMask(GL_TRUE); 
        glDepthFunc(GL_LESS); 
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
    }

}
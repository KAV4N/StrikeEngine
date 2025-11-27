#include "SkyboxRenderPass.h"
#include "StrikeEngine/Graphics/Renderer.h"
#include "StrikeEngine/Scene/World.h"
#include "StrikeEngine/Graphics/Skybox.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace StrikeEngine {

    SkyboxRenderPass::SkyboxRenderPass(Renderer& renderer)
        : RenderPass("SkyboxPass"), 
        mRenderer(renderer), 
        mSkybox(std::make_unique<Skybox>()) {
    }

    SkyboxRenderPass::~SkyboxRenderPass() {
        cleanup();
    }

    void SkyboxRenderPass::setup() {

    }

    void SkyboxRenderPass::cleanup() {

    }

    void SkyboxRenderPass::execute(const CameraRenderData& cameraData) {
  
        if (!cameraData.mSkyboxTexture) return;

        auto camera = cameraData.camera;
        auto cubeMap = cameraData.mSkyboxTexture;

        setupOpenGLState();

        glm::mat4 view = camera.getViewMatrix();
        view = glm::mat4(glm::mat3(view)); 
        glm::mat4 projection = camera.getProjectionMatrix();
        glm::mat4 viewProjection = projection * view;

        auto shader = mSkybox->getShader();
        

        shader->bind();
        shader->setMat4("uViewProjection", viewProjection);
        shader->setInt("uSkybox", 0);

        glBindVertexArray(mSkybox->getVAO());
        cubeMap->bind(0);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
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
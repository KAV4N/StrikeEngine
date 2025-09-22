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
        // Skybox resources are initialized in the Skybox class
    }

    void SkyboxRenderPass::cleanup() {
        // Skybox cleanup is handled by the Skybox class
    }

    void SkyboxRenderPass::execute(const CameraRenderData& cameraData) {
        if (!isEnabled() || !mSkybox || !mSkybox->isCubeMapValid() || !mSkybox->isShaderValid()) {
            return;
        }

        auto camera = cameraData.camera;

        // Setup viewport and scissor test
        const CameraComponent::Rect& viewportRect = camera.getViewportRect();
        GLint viewportX = static_cast<GLint>(viewportRect.x * mRenderer.getWidth());
        GLint viewportY = static_cast<GLint>(viewportRect.y * mRenderer.getHeight());
        GLsizei viewportWidth = static_cast<GLsizei>(viewportRect.width * mRenderer.getWidth());
        GLsizei viewportHeight = static_cast<GLsizei>(viewportRect.height * mRenderer.getHeight());

        glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
        glEnable(GL_SCISSOR_TEST);
        glScissor(viewportX, viewportY, viewportWidth, viewportHeight);

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

        glDisable(GL_SCISSOR_TEST);
    }

    void SkyboxRenderPass::setupOpenGLState() {
        // Enable depth testing but disable depth writing
        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL); // Skybox at max depth
        glDepthMask(GL_FALSE);  // Disable depth writing

        // Enable face culling
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT); // Cull front faces for skybox (inside of cube)
        glFrontFace(GL_CCW);
        
    }

    void SkyboxRenderPass::restoreOpenGLState() {
        glDepthMask(GL_TRUE); // Restore depth writing
        glDepthFunc(GL_LESS); // Restore default depth function
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
    }

}
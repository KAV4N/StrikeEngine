#include "SkyboxRenderPass.h"
#include "Renderer.h"
#include "StrikeEngine/Scene/Systems/RenderSystem.h"
#include "StrikeEngine/Scene/World.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace StrikeEngine {

    SkyboxRenderPass::SkyboxRenderPass(Renderer& renderer, const std::string& name)
        : RenderPass(name), mRenderer(renderer) {
    }

    SkyboxRenderPass::~SkyboxRenderPass() {
        cleanup();
    }

    void SkyboxRenderPass::setup() {
        // Initialize any resources needed for the skybox pass
        // The skybox geometry and resources are handled by the Skybox class itself
    }

    void SkyboxRenderPass::cleanup() {
        // Clean up any resources if necessary
        // The skybox resources are managed by the World class
    }

    void SkyboxRenderPass::execute(const CameraRenderData& cameraData) {
        if (!isEnabled()) return;

        auto camera = cameraData.camera;
        // Get the skybox from the world
        Skybox* skybox = World::get().getSkybox();
        if (!skybox || !skybox->isCubeMapValid() || !skybox->isShaderValid()) {
            return; // No valid skybox to render
        }

        // Setup viewport and scissor test using Renderer dimensions
        const CameraComponent::Rect& viewportRect = camera.getViewportRect();
        GLint viewportX = static_cast<GLint>(viewportRect.x * mRenderer.getWidth());
        GLint viewportY = static_cast<GLint>(viewportRect.y * mRenderer.getHeight());
        GLsizei viewportWidth = static_cast<GLsizei>(viewportRect.width * mRenderer.getWidth());
        GLsizei viewportHeight = static_cast<GLsizei>(viewportRect.height * mRenderer.getHeight());

        glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
        glEnable(GL_SCISSOR_TEST);
        glScissor(viewportX, viewportY, viewportWidth, viewportHeight);

        setupOpenGLState();


        skybox->bind();
        auto shader = skybox->getShader();
        if (shader && shader->isReady()) {

            glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
            glm::mat4 skyboxViewProjection = camera.getProjectionMatrix() * view;

            shader->setMat4("uViewProjection", skyboxViewProjection);
            shader->setInt("uSkybox", 0); // Cubemap texture unit
        }

        // Draw the skybox cube (36 indices for a cube)
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        skybox->unbind();

        glDisable(GL_SCISSOR_TEST);

        restoreOpenGLState();
    }

    void SkyboxRenderPass::setupOpenGLState() {

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glDisable(GL_CULL_FACE);

        glDepthMask(GL_FALSE);
    }

    void SkyboxRenderPass::restoreOpenGLState() {
    }

}
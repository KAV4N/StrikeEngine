#include "GeometryRenderPass.h"
#include "Renderer.h"
#include "StrikeEngine/Scene/Systems/RenderSystem.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace StrikeEngine {

    GeometryRenderPass::GeometryRenderPass(Renderer& renderer, const std::string& name)
        : RenderPass(name), mRenderer(renderer) {
    }

    GeometryRenderPass::~GeometryRenderPass() {
        cleanup();
    }

    void GeometryRenderPass::setup() {
        // Initialize any resources needed for the geometry pass
        // For now, we'll rely on OpenGL state setup in execute
    }

    void GeometryRenderPass::cleanup() {
        // Clean up any resources if necessary
    }

    void GeometryRenderPass::execute(const CameraRenderData& cameraData) {
        if (!isEnabled()) return;

        // Setup viewport and scissor test using Renderer dimensions
        const Rect& viewportRect = cameraData.camera->getViewportRect();
        GLint viewportX = static_cast<GLint>(viewportRect.x * mRenderer.getWidth());
        GLint viewportY = static_cast<GLint>(viewportRect.y * mRenderer.getHeight());
        GLsizei viewportWidth = static_cast<GLsizei>(viewportRect.width * mRenderer.getWidth());
        GLsizei viewportHeight = static_cast<GLsizei>(viewportRect.height * mRenderer.getHeight());

        glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
        glEnable(GL_SCISSOR_TEST);
        glScissor(viewportX, viewportY, viewportWidth, viewportHeight);

        setupOpenGLState();

        // Set view-projection matrix uniform
        for (const auto& renderItem : cameraData.renderItems) {
            if (!renderItem.mesh || !renderItem.material || !renderItem.mesh->isReady() || !renderItem.material->isReady()) {
                continue;
            }

            // Bind material and shader
            renderItem.material->bind();

            // Set the view-projection matrix
            auto shader = renderItem.material->getShader();
            if (shader) {
                shader->setMat4("uViewProjection", cameraData.viewProjectionMatrix);
                shader->setMat4("uModel", renderItem.worldMatrix);
            }

            // Bind vertex array object
            GLuint vao = renderItem.mesh->getVAO();
            if (vao == 0) {
                continue;
            }
            glBindVertexArray(vao);

            // Get submesh data
            const auto& subMesh = renderItem.mesh->getSubMeshes()[renderItem.subMeshIndex];

            // Draw the geometry
            glDrawElements(GL_TRIANGLES,
                subMesh.indexCount,
                GL_UNSIGNED_INT,
                (void*)(subMesh.startIndex * sizeof(uint32_t)));

            // Unbind resources
            glBindVertexArray(0);
            renderItem.material->unbind();
        }

        // Restore scissor test state
        glDisable(GL_SCISSOR_TEST);

        restoreOpenGLState();
    }

    void GeometryRenderPass::setupOpenGLState() {
        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        // Enable face culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glDepthFunc(GL_LESS);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void GeometryRenderPass::restoreOpenGLState() {
        // Disable states to clean up
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
    }

}
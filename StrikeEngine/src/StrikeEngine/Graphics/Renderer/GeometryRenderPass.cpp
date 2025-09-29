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
    }

    void GeometryRenderPass::cleanup() {
    }

    void GeometryRenderPass::bindPointLights(const std::vector<PointLightData>& pointLights, std::shared_ptr<Shader> shader) {
        shader->setInt("uNumPointLights", static_cast<int>(pointLights.size()));
        for (size_t i = 0; i < pointLights.size(); ++i) {
            const auto& light = pointLights[i];
            std::string base = "uPointLights[" + std::to_string(i) + "].";
            
            shader->setVec4(base + "position", light.position);        // xyz: position, w: unused
            shader->setVec4(base + "color", light.color);              // rgb: color, w: intensity
            shader->setVec4(base + "attenuation", light.attenuation);  // xyz: attenuation, w: radius
        }
    }

    void GeometryRenderPass::bindDirectionalLights(const std::vector<DirectionalLightData>& directionalLights, std::shared_ptr<Shader> shader) {
        shader->setInt("uNumDirLights", static_cast<int>(directionalLights.size()));
        for (size_t i = 0; i < directionalLights.size(); ++i) {
            const auto& light = directionalLights[i];
            std::string base = "uDirLights[" + std::to_string(i) + "].";
            
            shader->setVec4(base + "direction", light.direction);  // xyz: direction, w: unused
            shader->setVec4(base + "color", light.color);          // rgb: color, w: intensity
        }
    }

    void GeometryRenderPass::bindSpotLights(const std::vector<SpotLightData>& spotLights, std::shared_ptr<Shader> shader) {
        shader->setInt("uNumSpotLights", static_cast<int>(spotLights.size()));
        for (size_t i = 0; i < spotLights.size(); ++i) {
            const auto& light = spotLights[i];
            std::string base = "uSpotLights[" + std::to_string(i) + "].";
            
            shader->setVec4(base + "position", light.position);        // xyz: position, w: unused
            shader->setVec4(base + "direction", light.direction);      // xyz: direction, w: unused
            shader->setVec4(base + "color", light.color);              // rgb: color, w: intensity
            shader->setVec4(base + "anglesRadius", light.anglesRadius); // x: radius, y: innerCone, z: outerCone, w: unused
        }
    }

    void GeometryRenderPass::execute(const CameraRenderData& cameraData) {
        if (!isEnabled()) return;

        auto camera = cameraData.camera;
        glm::vec3 cameraPos = cameraData.cameraPosition; // Use stored camera position

        setupOpenGLState();

        for (const auto& renderItem : cameraData.renderItems) {
            if (!renderItem.mesh || !renderItem.material || !renderItem.mesh->isReady() || !renderItem.material->isReady()) {
                continue;
            }

            renderItem.material->bind();

            auto shader = renderItem.material->getShader();
 
            shader->setMat4("uViewProjection", camera.getViewProjectionMatrix());
            shader->setMat4("uModel", renderItem.worldMatrix);
            shader->setVec3("uViewPos", cameraPos);

            // Bind all light types
            bindPointLights(cameraData.pointLights, shader);
            bindDirectionalLights(cameraData.directionalLights, shader);
            bindSpotLights(cameraData.spotLights, shader);
            
            GLuint vao = renderItem.mesh->getVAO();
            if (vao == 0) {
                continue;
            }
            glBindVertexArray(vao);

            const auto& subMesh = renderItem.mesh->getSubMeshes()[renderItem.subMeshIndex];

            glDrawElements(GL_TRIANGLES,
                subMesh.indexCount,
                GL_UNSIGNED_INT,
                (void*)(subMesh.startIndex * sizeof(uint32_t)));

            glBindVertexArray(0);
            renderItem.material->unbind();
        }

        restoreOpenGLState();
    }

    void GeometryRenderPass::setupOpenGLState() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glDepthFunc(GL_LESS);
    }

    void GeometryRenderPass::restoreOpenGLState() {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
    }
}
#include "strikepch.h"
#include "CollisionRenderPass.h"
#include "StrikeEngine/Graphics/Renderer.h"
#include "StrikeEngine/Scene/World.h"
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/Components/PhysicsComponent.h"
#include "StrikeEngine/Graphics/Shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>


namespace StrikeEngine {

    CollisionRenderPass::CollisionRenderPass(Renderer& renderer)
        : RenderPass("CollisionWireframe"), mRenderer(renderer), mBoxVAO(0), mBoxVBO(0), mBoxEBO(0),
        mBoxIndexCount(0) {
    }

    CollisionRenderPass::~CollisionRenderPass() {
        cleanup();
    }

    void CollisionRenderPass::setup() {
        mShader = ShaderManager::get().getShader("collision.glsl");
        setupBoxGeometry();
    }

    void CollisionRenderPass::cleanup() {
        if (mBoxVAO) glDeleteVertexArrays(1, &mBoxVAO);
        if (mBoxVBO) glDeleteBuffers(1, &mBoxVBO);
        if (mBoxEBO) glDeleteBuffers(1, &mBoxEBO);
        mShader.reset();
    }

    void CollisionRenderPass::execute(const CameraRenderData& cameraData) {
        setupOpenGLState();

        Scene* scene = World::get().getScene();
        if (!scene) return;

        auto& registry = scene->getRegistry();

        mShader->bind();
        mShader->setMat4("uViewProjection", cameraData.camera.getViewProjectionMatrix());
        mShader->setVec4("uColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green wireframe
        
        auto view = registry.view<PhysicsComponent>();
        for (auto entity : view) {
            Entity ent(entity, scene);

            auto& physics = registry.get<PhysicsComponent>(entity);

            // Get the full world matrix including position, rotation, and scale
            glm::mat4 entWorldMatrix = ent.getWorldMatrix();
            
            // Get the collider's local size and center
            glm::vec3 size = physics.getSize();
            glm::vec3 center = physics.getCenter();

            // Create model matrix incorporating the entity's world transform and collider offset
            glm::mat4 model = entWorldMatrix;
            // Apply collider center offset in local space
            model = glm::translate(model, center);
            // Apply collider size (scaled by world scale)
            model = glm::scale(model, size);

            renderBox(model, cameraData.camera.getViewProjectionMatrix());
        }
        restoreOpenGLState();
    }

    void CollisionRenderPass::setupBoxGeometry() {
        // Define cube vertices (unit cube centered at origin)
        float vertices[] = {
            -0.5f, -0.5f, -0.5f, // 0
            0.5f, -0.5f, -0.5f, // 1
            0.5f,  0.5f, -0.5f, // 2
            -0.5f,  0.5f, -0.5f, // 3
            -0.5f, -0.5f,  0.5f, // 4
            0.5f, -0.5f,  0.5f, // 5
            0.5f,  0.5f,  0.5f, // 6
            -0.5f,  0.5f,  0.5f  // 7
        };

        uint32_t indices[] = {
            0, 1, 1, 2, 2, 3, 3, 0, // Front face
            4, 5, 5, 6, 6, 7, 7, 4, // Back face
            0, 4, 1, 5, 2, 6, 3, 7  // Connecting edges
        };

        mBoxIndexCount = 24;

        glGenVertexArrays(1, &mBoxVAO);
        glBindVertexArray(mBoxVAO);

        glGenBuffers(1, &mBoxVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mBoxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &mBoxEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBoxEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);
    }

    void CollisionRenderPass::renderBox(const glm::mat4& modelMatrix, const glm::mat4& viewProj) {
        mShader->setMat4("uModel", modelMatrix);
        glBindVertexArray(mBoxVAO);
        glDrawElements(GL_LINES, mBoxIndexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void CollisionRenderPass::setupOpenGLState() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void CollisionRenderPass::restoreOpenGLState() {
        glDisable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

} 
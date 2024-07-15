#include "strikepch.h"
#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

namespace StrikeEngine {
    Renderer* Renderer::s_Instance = nullptr;

    void Renderer::Create() {
        if (!s_Instance) {
            s_Instance = new Renderer();
        }
    }

    Renderer* Renderer::Get() {
        return s_Instance;
    }

    void Renderer::Destroy() {
        if (s_Instance) {
            delete s_Instance;
            s_Instance = nullptr;
        }
    }

    void Renderer::Init() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Renderer::BeginScene(Camera* camera) {
        m_ViewProjectionMatrix = camera->GetProjectionMatrix() * camera->GetViewMatrix();
    }

    void Renderer::EndScene() {
        // Any necessary cleanup after rendering a scene can go here
    }

    void Renderer::Update(Entity* entity, Shader* shader) {
        // Set the shader to use
        shader->Bind();

        // Calculate model-view-projection matrix
        glm::mat4 modelMatrix = entity->GetTransformationMatrix();
        glm::mat4 mvp = m_ViewProjectionMatrix * modelMatrix;

        // Pass the MVP matrix to the shader
        shader->LoadMatrix(shader->GetUniformLocation("u_MVP"), mvp);
    }

    void Renderer::Render(Entity* entity, Shader* shader) {
        // Bind the entity's model data (VAO, textures, etc.)
        glBindVertexArray(entity->GetModel()->GetVaoID());
        glBindTexture(GL_TEXTURE_2D, entity->GetModel()->GetTextureID()->GetID());

        // Draw the entity
        glDrawElements(GL_TRIANGLES, entity->GetModel()->GetVertexCount(), GL_UNSIGNED_INT, nullptr);

        // Unbind the VAO
        glBindVertexArray(0);
    }
}

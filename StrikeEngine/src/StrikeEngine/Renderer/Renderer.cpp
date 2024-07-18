#include "strikepch.h"
#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "StrikeEngine/Renderer/ShaderManager.h"
#include "StrikeEngine/Renderer/ModelManager.h"

namespace StrikeEngine {

    Renderer* Renderer::s_Instance = nullptr;

    void Renderer::Init() {
        ModelManager::Create();
        ShaderManager::Create();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Renderer::BeginScene(Camera* camera) {
        m_ViewProjectionMatrix = camera->GetProjectionMatrix() * camera->GetViewMatrix();
    }

    void Renderer::EndScene() {
        // Optional: Clean-up after rendering scene
    }

    void Renderer::Update(glm::mat4 modelMatrix, Shader* shader) {
        shader->Bind();
        glm::mat4 mvp = m_ViewProjectionMatrix * modelMatrix;
        shader->LoadMatrix(shader->GetUniformLocation("transform"), modelMatrix);
        shader->LoadMatrix(shader->GetUniformLocation("projection"), m_ViewProjectionMatrix);
    }

    void Renderer::SetDefaultTexture(const std::string& path) {
        if (m_DefaultTexture) {
            delete m_DefaultTexture;
        }
        m_DefaultTexture = new Texture(path);
    }

    void Renderer::Render(Model* model) {
        int textureUnit = 0;

        for (auto& part : model->GetParts()) {
            if (part->GetTextures().empty()) {
                s_Instance->m_DefaultTexture->Bind(GL_TEXTURE0);
            }
            else {
                for (auto& texture : part->GetTextures()) {
                    texture->Bind(GL_TEXTURE0 + textureUnit);
                    textureUnit++;
                }
            }

            glBindVertexArray(part->GetVaoID());
            glDrawElements(GL_TRIANGLES, part->GetVertexCount(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            if (part->GetTextures().empty() && s_Instance->m_DefaultTexture) {
                s_Instance->m_DefaultTexture->Unbind();
            }
            else {
                for (auto& texture : part->GetTextures()) {
                    texture->Unbind();
                }
            }
        }
    }

}

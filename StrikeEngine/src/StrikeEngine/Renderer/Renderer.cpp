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
        m_ViewMatrix = camera->GetViewMatrix();
    }

    void Renderer::EndScene() {
        
    }

    void Renderer::Update(const glm::mat4& transformationMatrix, Shader* shader, const std::vector<Light*>& lights, const glm::vec3& cameraPosition) {
        shader->Bind();
        shader->LoadTransformationMatrix(transformationMatrix);
        shader->LoadViewMatrix(m_ViewMatrix);
        shader->LoadProjectionMatrix(m_ViewProjectionMatrix); 
        shader->LoadViewPosition(cameraPosition);

        int lightIndex = 0;
        for (auto light : lights) {
            shader->LoadLight(lightIndex, light->GetPosition(), light->GetColor(), light->GetIntensity());
            lightIndex++;
        }
    }


    void Renderer::SetDefaultTexture(const std::string& path) {
        if (m_DefaultTexture) {
            delete m_DefaultTexture;
        }
        m_DefaultTexture = new Texture(path);
    }

    void Renderer::Render(Model* model) {
        for (auto& part : model->GetParts()) {
            if (part->GetTextures().empty()) {
                s_Instance->m_DefaultTexture->Bind(GL_TEXTURE0);
            }
            else {
                int textureUnit = 0;
                for (auto& texture : part->GetTextures()) {
                    texture->Bind(textureUnit);
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

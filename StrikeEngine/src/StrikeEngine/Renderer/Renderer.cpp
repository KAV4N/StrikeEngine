#include "strikepch.h"
#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "StrikeEngine/Renderer/ShaderManager.h"
#include "StrikeEngine/Renderer/ModelManager.h"
#include "StrikeEngine/Renderer/LightManager.h"

namespace StrikeEngine {

    Renderer* Renderer::s_Instance = nullptr;

    Renderer::Renderer() : m_DefaultTexture(nullptr), m_Skybox(nullptr), m_RenderSkybox(false) {}

    Renderer::~Renderer() {
        if (m_DefaultTexture) {
            delete m_DefaultTexture;
        }
        if (m_Skybox) {
            delete m_Skybox;
        }
    }

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
        ModelManager::Create();
        ShaderManager::Create();
        LightManager::Create();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Renderer::BeginScene(CameraComponent* camera) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_ViewProjectionMatrix = camera->ProjectionMatrix * camera->ViewMatrix;
        m_ViewMatrix = camera->ViewMatrix;
        m_ProjectionMatrix = camera->ProjectionMatrix;
    }



    void Renderer::EndScene() {
        Render();
    }

    void Renderer::SubmitScene(const entt::registry& registry) {
        auto view = registry.view<ModelComponent, TransformComponent>();
        for (auto entity : view) {
            auto& model = view.get<ModelComponent>(entity).model;
            auto& transform = view.get<TransformComponent>(entity).transformationMatrix;
            SubmitModel(model, transform);
        }
    }

    void Renderer::SubmitModel(Model* model, const glm::mat4& transformationMatrix) {
        Shader* shader = model->GetShader();
        m_RenderQueue[shader].push_back({ transformationMatrix, model, glm::vec3(0.0f) });  // cameraPosition will be updated later
    }

    void Renderer::SubmitSkybox(Skybox* skybox) {
        m_Skybox = skybox;
        m_RenderSkybox = true;
    }

    void Renderer::Render() {
        LightManager::Get()->BindLightsToShader();
        if (m_RenderSkybox) {
            RenderSkybox();
            m_RenderSkybox = false;
        }

        for (auto& pair : m_RenderQueue) {
            Shader* shader = pair.first;
            shader->Bind();

            for (const auto& command : pair.second) {
                BindShaderMVP(shader, command);
                RenderModelParts(shader, command);
            }

            shader->Unbind();
        }
        m_RenderQueue.clear();
    }

    void Renderer::RenderSkybox() {
        if (m_Skybox) {
            glDepthFunc(GL_LEQUAL);
            glDisable(GL_DEPTH_TEST);

            Shader* skyboxShader = m_Skybox->GetShader();
            skyboxShader->Bind();

            glm::mat4 view = glm::mat4(glm::mat3(m_ViewMatrix));
            skyboxShader->LoadUniform("view", view);
            skyboxShader->LoadUniform("projection", m_ProjectionMatrix);

            m_Skybox->Draw();

            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);
            skyboxShader->Unbind();
        }
    }

    void Renderer::BindShaderMVP(Shader* shader, const RenderCommand& command) {
        shader->LoadUniform("transform", command.transformationMatrix);
        shader->LoadUniform("view", m_ViewMatrix);
        shader->LoadUniform("projection", m_ProjectionMatrix);
        shader->LoadUniform("viewPosition", command.cameraPosition);
    }

    void Renderer::BindShaderMaterials(Shader* shader, ModelPart* part) {
        const Material& material = part->GetMaterial();
        shader->LoadUniform("material.ambient", material.ambient);
        shader->LoadUniform("material.diffuse", material.diffuse);
        shader->LoadUniform("material.specular", material.specular);
        shader->LoadUniform("material.shininess", material.shininess);
    }

    void Renderer::RenderModelParts(Shader* shader, const RenderCommand& command) {
        for (const auto& part : command.model->GetParts()) {
            BindShaderMaterials(shader, part);
            BindTextures(part);

            glBindVertexArray(part->GetVaoID());
            glDrawElements(GL_TRIANGLES, part->GetVertexCount(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            UnbindTextures(part);
        }
    }

    void Renderer::BindTextures(ModelPart* part) {
        int textureUnit = 0;

        if (part->GetTextures().empty() && m_DefaultTexture) {
            m_DefaultTexture->Bind(textureUnit);
        }
        else {
            for (const auto& texture : part->GetTextures()) {
                texture->Bind(textureUnit);
                textureUnit++;
            }
        }
    }

    void Renderer::UnbindTextures(ModelPart* part) {
        if (part->GetTextures().empty() && m_DefaultTexture) {
            m_DefaultTexture->Unbind();
        }
        else {
            for (const auto& texture : part->GetTextures()) {
                texture->Unbind();
            }
        }
    }

    void Renderer::SetDefaultTexture(const std::string& path) {
        if (m_DefaultTexture) {
            delete m_DefaultTexture;
        }
        m_DefaultTexture = new Texture(path);
    }
}

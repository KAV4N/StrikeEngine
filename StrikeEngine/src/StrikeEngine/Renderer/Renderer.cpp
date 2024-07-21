#include "strikepch.h"
#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "StrikeEngine/Renderer/ShaderManager.h"
#include "StrikeEngine/Renderer/ModelManager.h"

namespace StrikeEngine {

    Renderer* Renderer::s_Instance = nullptr;

    Renderer::Renderer() {}

    Renderer::~Renderer() {}

    void Renderer::Create() 
    {
        if (!s_Instance)
            s_Instance = new Renderer();
    }

    Renderer* Renderer::Get() 
    {
        return s_Instance;
    }

    void Renderer::Destroy() 
    {
        if (s_Instance) 
        {
            delete s_Instance;
            s_Instance = nullptr;
        }
    }

    void Renderer::Init() 
    {
        ModelManager::Create();
        ShaderManager::Create();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Renderer::BeginScene(Camera* camera) 
    {
        m_ViewProjectionMatrix = camera->GetProjectionMatrix() * camera->GetViewMatrix();
        m_ViewMatrix = camera->GetViewMatrix();
    }

    void Renderer::EndScene() 
    {
        Render();
    }

    void Renderer::SubmitScene(const std::vector<Entity*>& entities, const std::vector<Light>& lights, const glm::vec3& cameraPosition) 
    {
        for (auto entity : entities) 
        {
            entity->IncreaseRotation(glm::vec3(0.f, 0.f, 1.f)); // TODO: REMOVE, ONLY FOR TESTING
            Submit(entity->GetTransformationMatrix(), entity->GetModel(), lights, cameraPosition);
        }
    }

    void Renderer::Submit(const glm::mat4& transformationMatrix, Model* model, const std::vector<Light>& lights, const glm::vec3& cameraPosition) 
    {
        Shader* shader = model->GetShader();
        m_RenderQueue[shader].push_back({ transformationMatrix, model, lights, cameraPosition });
    }

    void Renderer::Render() 
    {
        for (auto& pair : m_RenderQueue) 
        {
            Shader* shader = pair.first;
            shader->Bind();

            for (const auto& command : pair.second) 
            {
                BindShaderAndSetUniforms(shader, command);
                RenderModelParts(command);
            }

            shader->Unbind();
        }

        m_RenderQueue.clear();
    }

    void Renderer::BindShaderAndSetUniforms(Shader* shader, const RenderCommand& command) 
    {
        shader->LoadTransformationMatrix(command.transformationMatrix);
        shader->LoadViewMatrix(m_ViewMatrix);
        shader->LoadProjectionMatrix(m_ViewProjectionMatrix);
        shader->LoadViewPosition(command.cameraPosition);

        int lightIndex = 0;
        for (auto light : command.lights) 
        {
            shader->LoadLight(lightIndex, light.position, light.color, light.intensity);
            lightIndex++;
        }

        for (auto part : command.model->GetParts()) 
        {
            const Material& material = part->GetMaterial();
            shader->LoadMaterialAmbient(material.ambient);
            shader->LoadMaterialDiffuse(material.diffuse);
            shader->LoadMaterialSpecular(material.specular);
            shader->LoadMaterialShininess(material.shininess);
        }
    }

    void Renderer::RenderModelParts(const RenderCommand& command) 
    {
        for (auto& part : command.model->GetParts()) 
        {
            BindTextures(part);

            glBindVertexArray(part->GetVaoID());
            glDrawElements(GL_TRIANGLES, part->GetVertexCount(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            UnbindTextures(part);
        }
    }

    void Renderer::BindTextures(ModelPart* part) 
    {
        int textureUnit = 0;

        if (part->GetTextures().empty() && m_DefaultTexture) 
        {
            m_DefaultTexture->Bind(textureUnit);
        }
        else {
            for (auto& texture : part->GetTextures()) 
            {
                texture->Bind(textureUnit);
                textureUnit++;
            }
        }
    }

    void Renderer::UnbindTextures(ModelPart* part) 
    {
        if (part->GetTextures().empty() && m_DefaultTexture) 
        {
            m_DefaultTexture->Unbind();
        }
        else {
            for (auto& texture : part->GetTextures()) 
            {
                texture->Unbind();
            }
        }
    }
    
    void Renderer::SetDefaultTexture(const std::string& path) 
    {
        if (m_DefaultTexture) 
        {
            delete m_DefaultTexture;
            m_DefaultTexture = nullptr;
        }
        m_DefaultTexture = new Texture(path);
    }
    
}

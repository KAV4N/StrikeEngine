#include "strikepch.h"
#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "StrikeEngine/Renderer/ShaderManager.h"
#include "StrikeEngine/Renderer/ModelManager.h"
#include "StrikeEngine/Renderer/LightManager.h"
#include "StrikeEngine/Scene/Components/ModelComponent.h"
#include "StrikeEngine/Renderer/FrameBufferManager.h"

//TODO: REMOVE AFTER TESTING
#include <StrikeEngine/Scene/Systems/TransformSystem.h> 
//---------------------------------------------

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
        FrameBufferManager::Create();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Renderer::BeginScene(CameraComponent* camera) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_CameraViewProjectionMatrix = camera->ProjectionMatrix * camera->ViewMatrix;
        m_CameraViewMatrix = camera->ViewMatrix;
        m_CameraProjectionMatrix = camera->ProjectionMatrix;
        m_CameraPosition = camera->Position;


    }



    void Renderer::EndScene() {
        Render();
    }

    void Renderer::SubmitScene(Scene* scene) {
        const auto view = scene->GetRegistry().view<ModelComponent, TransformComponent>();
        for (auto entityHandle : view) {
            Entity entity{ entityHandle, scene };

            //TO DO: REMOVE AFTER TESTING-----------------------------------------
            TransformSystem::IncreaseRotation(entity, glm::uvec3(0.f, 0.f, 1.f));
            auto& modelComp = entity.GetComponent<ModelComponent>();
            modelComp.parts[0].rotation += glm::vec3(0.0f, 0.0f, 45.0f);
            //---------------------------------------------------------------------

            auto& transform = entity.GetComponent<TransformComponent>().transformationMatrix;
            SubmitEntity(entity, transform);
        }
    }

    void Renderer::SubmitEntity(Entity entity, const glm::mat4& transformationMatrix) {
        auto& modelComp = entity.GetComponent<ModelComponent>();
        Shader* shader = modelComp.model->GetShader();
        m_RenderQueue[shader].push_back({ transformationMatrix, entity, m_CameraPosition });
    }

    void Renderer::SubmitSkybox(Skybox* skybox) {
        m_Skybox = skybox;
        m_RenderSkybox = true;
    }
    void RenderShadowMaps() {
        LightManager::Get()->UpdateShadowMaps();
    }

    void Renderer::Render() {
        LightManager::Get()->BindLightsToShader();
        RenderShadowMaps();
        RenderSkybox();
        
        for (auto& pair : m_RenderQueue) {
            Shader* shader = pair.first;
            shader->Bind();
            
            BindShadowMapsToShader(shader);
            for (const auto& command : pair.second) {
                BindShaderMVP(shader, command);
                RenderModelParts(shader, command);
            }
            
            shader->Unbind();
        }
        m_RenderQueue.clear();
    }

    void Renderer::BindShadowMapsToShader(Shader* shader) {
       
    }

    void Renderer::RenderSkybox() {
        if (m_Skybox) {
            glDepthFunc(GL_LEQUAL);
            glDisable(GL_DEPTH_TEST);

            Shader* skyboxShader = m_Skybox->GetShader();
            skyboxShader->Bind();

            glm::mat4 view = glm::mat4(glm::mat3(m_CameraViewMatrix));

            skyboxShader->LoadUniform("MVP", m_CameraProjectionMatrix * view);

            m_Skybox->Draw();

            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);
            skyboxShader->Unbind();
        }
    }

    void Renderer::BindShaderMVP(Shader* shader, const RenderCommand& command) {
        //shader->LoadUniform("transform", command.transformationMatrix);
        shader->LoadUniform("MVP", m_CameraProjectionMatrix * m_CameraViewMatrix);
        shader->LoadUniform("viewPosition", m_CameraPosition);
    }

    void Renderer::BindShaderMaterials(Shader* shader, ModelPart* part) {
        const Material& material = part->GetMaterial();
        shader->LoadUniform("material.ambient", material.ambient);
        shader->LoadUniform("material.diffuse", material.diffuse);
        shader->LoadUniform("material.specular", material.specular);
        shader->LoadUniform("material.shininess", material.shininess);
    }

    void Renderer::RenderModelParts(Shader* shader, const RenderCommand& command) {
        const auto& modelComp = command.entity.GetComponent<ModelComponent>();
        for (const auto& partComp : modelComp.parts) {
            BindShaderMaterials(shader, partComp.part);
            BindTextures(partComp.part);

            glm::mat4 partTransform = command.transformationMatrix * partComp.localTransform;
            shader->LoadUniform("transform", partTransform);

            glBindVertexArray(partComp.part->GetVaoID());
            glDrawElements(GL_TRIANGLES, partComp.part->GetVertexCount(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            UnbindTextures(partComp.part);
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
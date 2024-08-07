
#include "strikepch.h"
#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "StrikeEngine/Renderer/Managers/ShaderManager.h"
#include "StrikeEngine/Renderer/Managers/ModelManager.h"
#include "StrikeEngine/Renderer/Managers/LightManager.h"
#include "StrikeEngine/Scene/Components/ModelComponent.h"
#include "StrikeEngine/Renderer/Core/VisibilityCuller.h"

// TODO: REMOVE AFTER TESTING
#include <StrikeEngine/Scene/Systems/TransformSystem.h>
// ---------------------------------------------

namespace StrikeEngine {

    Renderer* Renderer::s_Instance = nullptr;

    Renderer::Renderer()
        : m_DefaultTexture(nullptr), m_Skybox(nullptr), m_RenderSkybox(false),
        m_Width(1280), m_Height(720), m_FullScreenQuadShader(nullptr) {
        SetupFullScreenQuad();
    }

    Renderer::~Renderer() {
        if (m_DefaultTexture) {
            delete m_DefaultTexture;
        }
        if (m_Skybox) {
            delete m_Skybox;
        }

        glDeleteFramebuffers(1, &m_Framebuffer);
        glDeleteTextures(1, &m_ColorAttachment);
        glDeleteRenderbuffers(1, &m_DepthAttachment);

        glDeleteVertexArrays(1, &m_QuadVAO);
        glDeleteBuffers(1, &m_QuadVBO);

        delete m_FullScreenQuadShader;
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
        m_FullScreenQuadShader = ShaderManager::Get()->GetShader("ScreenShader");
        CreateFramebuffer();
    }

    void Renderer::CreateFramebuffer() {
        // Delete existing framebuffer if it exists
        if (m_Framebuffer) {
            glDeleteFramebuffers(1, &m_Framebuffer);
            glDeleteTextures(1, &m_ColorAttachment);
            glDeleteRenderbuffers(1, &m_DepthAttachment);
        }

        // Create new framebuffer
        glGenFramebuffers(1, &m_Framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

        // Color attachment
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

        // Depth attachment
        glGenRenderbuffers(1, &m_DepthAttachment);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer is not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    void Renderer::BeginScene(CameraComponent* camera) {
        LightManager::Get()->BindLightsToShader();


        m_CameraViewProjectionMatrix = camera->ProjectionMatrix * camera->ViewMatrix;
        m_CameraViewMatrix = camera->ViewMatrix;
        m_CameraProjectionMatrix = camera->ProjectionMatrix;
        m_CameraPosition = camera->Position;
    }

    void Renderer::EndScene() 
    {
        
        RenderShadowMaps();
        
        glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
        glViewport(0, 0, m_Width, m_Height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Render();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_FullScreenQuadShader->Bind();
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        RenderFullScreenQuad();
        glBindTexture(GL_TEXTURE_2D, 0);
        m_FullScreenQuadShader->Unbind();
        
        /*
        //TEST AREA
        RenderShadowMaps();
        m_RenderQueue.clear();
        RenderShadowMapTexture();
        */
        
    }

    void Renderer::SubmitScene(Scene* scene) {
        const auto view = scene->GetRegistry().view<ModelComponent, TransformComponent>();
        for (auto entityHandle : view) {
            Entity entity{ entityHandle, scene };
            
            // TODO: REMOVE AFTER TESTING-----------------------------------------
            TransformSystem::IncreaseRotation(entity, glm::uvec3(0.f, 0.f, 1.f));
            auto& modelComp = entity.GetComponent<ModelComponent>();
            modelComp.parts[0].rotation += glm::vec3(0.0f, 0.0f, 5.0f);
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
    
    void Renderer::RenderShadowMaps() {
        LightManager::Get()->UpdateShadowMaps(m_RenderQueue);
    }


    void Renderer::Render() {

        RenderSkybox();

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

            glm::mat4 view = glm::mat4(glm::mat3(m_CameraViewMatrix));
            skyboxShader->LoadUniform("MVP", m_CameraProjectionMatrix * view);

            m_Skybox->Draw();

            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);
            skyboxShader->Unbind();
        }
    }

    void Renderer::BindShaderMVP(Shader* shader, const RenderCommand& command) {
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
            glm::mat4 partTransform = command.transformationMatrix * partComp.localTransform;

            if (VisibilityCuller::IsVisible(partComp.part->GetAABB(), partTransform, m_CameraViewProjectionMatrix)) {
                BindShaderMaterials(shader, partComp.part);
                BindTextures(partComp.part);

                shader->LoadUniform("transform", partTransform);

                partComp.part->Draw();

                UnbindTextures(partComp.part);
            }
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

    void Renderer::SetupFullScreenQuad() {
        float vertices[] = {
            // positions   // texture Coords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &m_QuadVAO);
        glGenBuffers(1, &m_QuadVBO);
        glBindVertexArray(m_QuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }

    void Renderer::RenderFullScreenQuad() {
        glBindVertexArray(m_QuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void Renderer::RenderShadowMapTexture() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_Width, m_Height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader* shadowMapShader = ShaderManager::Get()->GetShader("ShadowMapTestShader");
        shadowMapShader->Bind();

        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, LightManager::Get()->GetShadowAtlas()->GetTextureID());
        shadowMapShader->LoadUniform("shadowAtlas", 0);


        RenderFullScreenQuad();

        //glBindTexture(GL_TEXTURE_2D, 0);
        //shadowMapShader->Unbind();
    }


}
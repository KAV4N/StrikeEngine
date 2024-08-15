
#include "strikepch.h"
#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "StrikeEngine/Renderer/Managers/ShaderManager.h"
#include "StrikeEngine/Renderer/Managers/ModelManager.h"
#include "StrikeEngine/Renderer/Managers/LightManager.h"
#include "StrikeEngine/Scene/Components/ModelComponent.h"
#include "StrikeEngine/Renderer/Core/VisibilityCuller.h"


#include "StrikeEngine/Renderer/Renderer3D/ShadowRenderer.h"
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
        ShadowRenderer::Create();

        LightManager::Get()->BindLights();

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

    void Renderer::SubmitCommand(RenderCommand renderCommand) {
        m_RenderQueue.push(renderCommand);
    }

    void Renderer::SubmitSkybox(Skybox* skybox) {
        m_Skybox = skybox;
        m_RenderSkybox = true;
    }
    
    void Renderer::RenderShadowMaps() {
        //ShadowRenderer::Get()->UpdateShadowMaps(m_RenderQueue);
    }


    void Renderer::Render() {
        RenderShadowMaps();
        


        while (!m_RenderQueue.empty())
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
            glViewport(0, 0, m_Width, m_Height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            LightManager::Get()->UpdateSSBOs();

            RenderCommand renderCommand = m_RenderQueue.front();
            CameraComponent camera = renderCommand.cameraEntity.GetComponent<CameraComponent>();
            
            glm::mat4 cameraProjection = camera.ProjectionMatrix;
            glm::mat4 cameraView = camera.ViewMatrix;
            glm::vec3 cameraPosition = camera.Position;

            RenderSkybox(cameraView, cameraProjection);

            for (auto it = renderCommand.shaderEntityMap.begin(); it != renderCommand.shaderEntityMap.end(); ++it) {
                Shader* shader = it->first;
                shader->Bind();
                shader->LoadUniform("MVP", cameraProjection * cameraView);
                shader->LoadUniform("viewPosition", cameraPosition);

                for (Entity modelEntity : it->second) {
                    auto& modelComp = modelEntity.GetComponent<ModelComponent>();
                    glm::mat4 modelTransform = modelEntity.GetComponent<TransformComponent>().transformationMatrix;
                    
                    for (auto& partEntity : modelComp.parts) {
                        auto& modelPartComp = partEntity.GetComponent<ModelPartComponent>();
                        auto& partTransform = partEntity.GetComponent<TransformComponent>().transformationMatrix;
                        auto& materialComp = partEntity.GetComponent<MaterialComponent>();

                        shader->LoadUniform("material.baseColor", materialComp.baseColor);
                        shader->LoadUniform("material.ambient", materialComp.ambient);
                        shader->LoadUniform("material.diffuse", materialComp.diffuse);
                        shader->LoadUniform("material.shininess", materialComp.shininess);
                        shader->LoadUniform("material.specular", materialComp.specular);

                        glm::mat4 modelMatrix = modelTransform * partTransform;
                        shader->LoadUniform("transform", modelMatrix);

                        auto& textures = partEntity.GetComponent<TextureComponent>().textures;
                        if (!textures.empty()) {
                            textures[0]->Bind(0);
                        }
                        else {
                            m_DefaultTexture->Bind();
                        }

                        shader->LoadUniform("ourTexture", 0);

                        glBindVertexArray(modelPartComp.vaoID);
                        glDrawElements(GL_TRIANGLES, modelPartComp.vertexCount, GL_UNSIGNED_INT, 0);
                        glBindVertexArray(0);
                    }

                
                }


                shader->Unbind();
            }
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, m_Width, m_Height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            m_FullScreenQuadShader->Bind();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
            m_FullScreenQuadShader->LoadUniform("screenTexture", 0);
            RenderFullScreenQuad();
            glBindTexture(GL_TEXTURE_2D, 0);
            m_FullScreenQuadShader->Unbind();

            m_RenderQueue.pop();
        }
        

        
    }

    void Renderer::RenderSkybox(glm::mat4 cameraView, glm::mat4 cameraProjection) {
        if (m_Skybox) {
            glDepthFunc(GL_LEQUAL);
            glDisable(GL_DEPTH_TEST);

            Shader* skyboxShader = m_Skybox->GetShader();
            skyboxShader->Bind();

            glm::mat4 view = glm::mat4(glm::mat3(cameraView));
            skyboxShader->LoadUniform("MVP", cameraProjection * view);

            m_Skybox->Draw();

            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);
            skyboxShader->Unbind();
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
        glBindTexture(GL_TEXTURE_2D, ShadowRenderer::Get()->GetTextureID());
        shadowMapShader->LoadUniform("shadowAtlas", 0);


        RenderFullScreenQuad();

        //glBindTexture(GL_TEXTURE_2D, 0);
        //shadowMapShader->Unbind();
    }


}
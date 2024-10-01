
#include "strikepch.h"
#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "StrikeEngine/Graphics/Managers/ShaderManager.h"
#include "StrikeEngine/Graphics/Managers/ModelManager.h"
#include "StrikeEngine/Graphics/Managers/LightManager.h"
#include "StrikeEngine/Scene/Components/ModelComponent.h"
#include "StrikeEngine/Graphics/Core/VisibilityCuller.h"

#include "StrikeEngine/Graphics/Renderer/ShadowRenderer.h"
// TODO: REMOVE AFTER TESTING
#include <StrikeEngine/Scene/Systems/TransformSystem.h>
#include <imgui.h>
#include <StrikeEngine/Core/Application.h>
// ---------------------------------------------

namespace StrikeEngine {

    Renderer* Renderer::s_Instance = nullptr;

    Renderer::Renderer()
        : m_Skybox(nullptr),
        m_QuadShader(nullptr),
        m_QuadScreenShader(nullptr)
    {
    }

    Renderer::~Renderer() {
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

        m_QuadShader = ShaderManager::Get()->GetShader("ScreenShader");
        //m_QuadScreenShader = ShaderManager::Get()->GetShader("QuadScreen");
        InitQuad();
        //InitScreenQuad();
        //glViewport(0, 0, 1280, 720);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }


    void Renderer::InitQuad() {
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
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glBindVertexArray(0);
    
    }

    void Renderer::InitScreenQuad()
    {
        float aspect_ratio = 16.0f / 9.0f;
        float quad_height = 1.0f;
        float quad_width = quad_height * aspect_ratio;
        float vertices[] = {
            // positions        // texture coords
            -quad_width / 2, -quad_height / 2, 0.0f,  0.0f, 0.0f,
             quad_width / 2, -quad_height / 2, 0.0f,  1.0f, 0.0f,
             quad_width / 2,  quad_height / 2, 0.0f,  1.0f, 1.0f,
            -quad_width / 2,  quad_height / 2, 0.0f,  0.0f, 1.0f
        };
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

    }
    

    void Renderer::SubmitMesh(Shader* shader,RenderItem renderItem) {
        s_Instance->m_ShaderBatch[shader].push_back(renderItem);
    }


    void Renderer::SubmitSkybox(Skybox* skybox) {
        s_Instance->m_Skybox = skybox;
    }

    void Renderer::Present(Camera* camera)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        LightManager::Get()->UpdateSSBOs();

        const glm::mat4 cameraView = camera->GetViewMatrix();
        const glm::mat4 cameraProjection = camera->GetProjectionMatrix();
        const glm::vec3 cameraPosition = camera->GetPosition();
        const glm::mat4 viewProjection = cameraProjection * cameraView;


        RenderSkybox(cameraView, cameraProjection);
        RenderScene(viewProjection, cameraPosition);

    }


    void Renderer::RenderScene(const glm::mat4& viewProjection, const glm::vec3& cameraPosition) {
        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);
        for (const auto& [shader, renderItems] : m_ShaderBatch) {
            shader->Bind();
            shader->LoadUniform("u_ViewProjection", viewProjection);
            shader->LoadUniform("u_CameraPosition", cameraPosition);

            shader->LoadUniform("u_NumSpotLights", LightManager::Get()->GetSpotCount());
            shader->LoadUniform("u_NumDirLights", LightManager::Get()->GetDirectionalCount());
            shader->LoadUniform("u_NumPointLights", LightManager::Get()->GetPointCount());

            for (const auto& rendItem : renderItems) {
                shader->LoadUniform("u_Model", rendItem.Transform);

                rendItem.MeshObj->Draw(shader);
            }
        }
        glUseProgram(0);
    }

    void Renderer::RenderSkybox(const glm::mat4& cameraView, const glm::mat4& cameraProjection) {
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_DEPTH_TEST);
            
        Shader* skyboxShader = m_Skybox->GetShader();

        skyboxShader->Bind();
        glm::mat4 view = glm::mat4(glm::mat3(cameraView));
        skyboxShader->LoadUniform("u_ViewProjection", cameraProjection * view);
        m_Skybox->Draw();
        skyboxShader->Unbind();

    }

    void Renderer::DrawTexturedQuad(const glm::vec2& position, const glm::vec2& size, const GLuint& texture) {

        glDisable(GL_DEPTH_TEST);
        glm::mat4 model = glm::mat4(1.0f);        
        model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
        model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));
        
        m_QuadShader->Bind();
      
        m_QuadShader->LoadUniform("u_Model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        m_QuadShader->LoadUniform("u_DiffuseTexture", 0);

        glBindVertexArray(m_QuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        m_QuadShader->Unbind();
    }
    /*
    void Renderer::DrawScreenQuad(const glm::vec2& position, const glm::vec2& size, const GLuint& texture)
    {

        m_QuadScreenShader->Bind();
        float aspect = (float)m_Width / (float)m_Height;
        glm::mat4 projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
        m_QuadScreenShader->LoadUniform("projection", projection);

        glm::mat4 model = glm::mat4(1.0f);
        m_QuadScreenShader->LoadUniform("model", model);



        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        m_QuadShader->LoadUniform("texture", 0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);


        m_QuadScreenShader->Unbind();

    }*/

    void Renderer::BindDefaultFrameBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::Flush()
    {
        s_Instance->m_ShaderBatch.clear();
    }

    void Renderer::Resize(GLuint width, GLuint height) {
        glViewport(0, 0, width, height);
    }


}
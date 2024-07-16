#include "strikepch.h"
#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "ShaderManager.h"
#include "TextureManager.h"

namespace StrikeEngine {
    Renderer* Renderer::s_Instance = nullptr;

    void Renderer::Create() 
    {
        if (!s_Instance) 
        {
            s_Instance = new Renderer();
        }
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
        ShaderManager::Create();
        TextureManager::Create();
        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
    }

    void Renderer::BeginScene(Camera* camera) 
    {
        m_ViewProjectionMatrix = camera->GetProjectionMatrix() * camera->GetViewMatrix();
    }

    void Renderer::EndScene() 
    {
       
    }

    void Renderer::Update(Entity* entity, Shader* shader) 
    {
        shader->Bind();
        glm::mat4 modelMatrix = entity->GetTransformationMatrix();
        glm::mat4 mvp = m_ViewProjectionMatrix * modelMatrix;
        shader->LoadMatrix(shader->GetUniformLocation("transform"), modelMatrix);
        shader->LoadMatrix(shader->GetUniformLocation("projection"), m_ViewProjectionMatrix);
    }


    void Renderer::Render(Entity* entity, Shader* shader) 
    {

        glBindVertexArray(entity->GetModel()->GetVaoID());
        glBindTexture(GL_TEXTURE_2D, entity->GetModel()->GetTextureID()->GetID());


        glDrawElements(GL_TRIANGLES, entity->GetModel()->GetVertexCount(), GL_UNSIGNED_INT, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }
}

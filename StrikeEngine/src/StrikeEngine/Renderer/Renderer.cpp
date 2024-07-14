#include "strikepch.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"

namespace StrikeEngine
{
    Renderer* Renderer::s_Instance = nullptr;

    const float Renderer::s_FOW = 70.f;
    const float Renderer::s_NEAR_PLANE = 0.1f;
    const float Renderer::s_FAR_PLANE = 1000.f;

    Renderer& Renderer::Get()
    {
        if (s_Instance == nullptr)
        {
            STRIKE_CORE_ASSERT(true, "instance was not created!");
        }
        return *s_Instance;
    }

    void Renderer::Create()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = new Renderer();
        }
    }

    void Renderer::Shutdown()
    {
        if (s_Instance) {
            delete s_Instance;
            s_Instance = nullptr;
        }
    }

    void Renderer::Init()
    {
        ShaderManager::Create();
        TextureManager::Create();
        CreateProjectionMatrix();
    }

    void Renderer::CreateProjectionMatrix()
    {
        float aspectRatio = 16.0f / 9.0f; 
        m_ProjectionMatrix = glm::perspective(glm::radians(s_FOW), aspectRatio, s_NEAR_PLANE, s_FAR_PLANE);
    }

    void Renderer::Update(Entity* entity, Shader* shader)
    {
        Clear();
        Model* model = entity->GetModel();
        glBindTexture(GL_TEXTURE_2D, model->GetTextureID()->getID());
        glBindVertexArray(model->GetVaoID());

        shader->GetAllUniformLocations();
        shader->LoadTransformationMatrix(entity->GetTransformationMatrix());

        glDrawElements(GL_TRIANGLES, model->GetVertexCount(), GL_UNSIGNED_INT, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }

    void Renderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

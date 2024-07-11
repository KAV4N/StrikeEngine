#include "strikepch.h"
#include "Renderer.h"
#include "ShaderManager.h"

namespace StrikeEngine
{
    Renderer* Renderer::s_Instance = nullptr;

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
    }

    void Renderer::Render(RawModel* model)
    {
        Clear();
        glBindVertexArray(model->GetVaoID());
        glDrawElements(GL_TRIANGLES, model->GetVertexCount(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }


    void Renderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

#include "strikepch.h"
#include "RenderManager.h"
#include "ShaderManager.h"

namespace StrikeEngine
{
    RenderManager* RenderManager::s_Instance = nullptr;

    RenderManager& RenderManager::Get()
    {
        if (s_Instance == nullptr)
        {
            STRIKE_CORE_ASSERT(true, "instance was not created!");
        }
        return *s_Instance;
    }

    void RenderManager::Create()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = new RenderManager();
        }

    }

    void RenderManager::Shutdown()
    {

    }

    void RenderManager::Init()
    {
        ShaderManager::Create();
    }

    void RenderManager::Render(Model* model)
    {
        Clear();
        glBindVertexArray(model->GetId());
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLES, 0, model->GetVertexCount());
        glDisableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    void RenderManager::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

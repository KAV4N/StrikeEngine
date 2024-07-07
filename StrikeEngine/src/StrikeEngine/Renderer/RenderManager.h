#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <gl/GL.h>
#include <StrikeEngine/Renderer/Entity/Model.h>

class GLFWwindow;

namespace StrikeEngine
{
    class RenderManager
    {
    public:
        static RenderManager& Get();
        static void Create();
        static void Shutdown();
        static void Render(Model* model);
        static void Clear();
        void Init();
        
        

    private:
        RenderManager() = default;
        ~RenderManager() = default;

        RenderManager(const RenderManager&) = delete;
        RenderManager& operator=(const RenderManager&) = delete;

        static RenderManager* s_Instance;
    };
}

#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "Model.h"

class GLFWwindow;

namespace StrikeEngine
{
    class Renderer
    {
    public:
        static Renderer& Get();
        static void Create();
        static void Shutdown();
        static void Update(Model* model);
        static void Clear();
        void Init();
        
        

    private:
        Renderer() = default;
        ~Renderer() = default;

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        static Renderer* s_Instance;
    };
}

#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "StrikeEngine/Scene/Entity.h"

class GLFWwindow;

namespace StrikeEngine
{
    class Renderer
    {
    public:
        static Renderer& Get();
        static void Create();
        static void Shutdown();
        static void Update(Entity* entity, Shader* shader);
        static void Clear();
        void Init();

    private:
        void CreateProjectionMatrix();

    private:
        Renderer() = default;
        ~Renderer() = default;
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        static const float s_FOW;
        static const float s_NEAR_PLANE;
        static const float s_FAR_PLANE;
        glm::mat4 m_ProjectionMatrix;

        static Renderer* s_Instance;
    };
}

#pragma once


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

#include <StrikeEngine/Scene/Camera.h>
#include <StrikeEngine/Scene/Entity.h>


namespace StrikeEngine {
    class Renderer {
    public:
        void BeginScene(Camera* camera);
        void EndScene();
        void Update(Entity* entity, Shader* shader);

    public:
        static void Create();
        static Renderer* Get();
        static void Destroy();
        static void Init();

        static void Render(Entity* entity, Shader* shader);

    private:
        Renderer() = default;
        ~Renderer() = default;

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        static Renderer* s_Instance;

        glm::mat4 m_ViewProjectionMatrix;
    };
}

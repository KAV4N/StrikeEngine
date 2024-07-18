#pragma once

#include <glm/glm.hpp>
#include "StrikeEngine/Scene/Camera.h"
#include "StrikeEngine/Renderer/Shader.h"
#include "StrikeEngine/Renderer/Model.h"
#include "StrikeEngine/Renderer/Texture.h"

namespace StrikeEngine {

    class Renderer
    {
    public:
        static void Create() {
            if (!s_Instance)
                s_Instance = new Renderer();
        }

        static Renderer* Get() {
            return s_Instance;
        }

        static void Destroy() {
            if (s_Instance) {
                delete s_Instance;
                s_Instance = nullptr;
            }
        }

        static void Init();

        void BeginScene(Camera* camera);
        void EndScene();
        void Update(glm::mat4 modelMatrix, Shader* shader);
        void SetDefaultTexture(const std::string& path);
        void Render(Model* model);

    private:
        Renderer() {}
        ~Renderer() {}

        static Renderer* s_Instance;
        glm::mat4 m_ViewProjectionMatrix;
        Texture* m_DefaultTexture = nullptr;
    };

}

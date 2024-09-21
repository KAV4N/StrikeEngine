#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <entt/entt.hpp>
#include "StrikeEngine/Scene/Camera.h"
#include "StrikeEngine/Graphics/Core/Shader.h"
#include "StrikeEngine/Graphics/Core/Model.h"
#include "StrikeEngine/Graphics/Core/Texture.h"
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include "StrikeEngine/Scene/Components/TransformComponent.h"
#include <StrikeEngine/Scene/Components/ModelComponent.h>
#include "StrikeEngine/Graphics/Core/Skybox.h"
#include "StrikeEngine/Scene/RenderCommand.h"
#include <queue>
#include <StrikeEngine/Graphics/Core/FrameBuffer.h>

namespace StrikeEngine {



    class Renderer {
    public:
        friend class LightManager;

        static void Create();
        static Renderer* Get();
        static void Destroy();
        static void SubmitSkybox(Skybox* skybox);
        static void SubmitMesh(Shader* shader, RenderItem renderItem);
        static void Flush(); //Clear batch

        static void BindDefaultFrameBuffer();

    public:
        void Init();
        void Resize(GLuint width, GLuint height);
        void Present(Camera* camera);
        void DrawTexturedQuad(const glm::vec2& position, const glm::vec2& size, const GLuint& texture);
    private:
        Renderer();
        ~Renderer();

        void InitQuad();

        void RenderSkybox(const glm::mat4& cameraView, const glm::mat4& cameraProjection);
        void RenderScene(const glm::mat4& viewProjection, const glm::vec3& cameraPosition);
    private:
        static Renderer* s_Instance;
        
        std::unordered_map<Shader*, std::vector<RenderItem>> m_ShaderBatch;
        Skybox* m_Skybox;

        GLuint m_QuadVAO, m_QuadVBO, m_QuadIBO;
        Shader* m_QuadShader;
    };
}
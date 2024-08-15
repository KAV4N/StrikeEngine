#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <entt/entt.hpp>
#include "StrikeEngine/Scene/Camera.h"
#include "StrikeEngine/Renderer/Core/Shader.h"
#include "StrikeEngine/Renderer/Core/Model.h"
#include "StrikeEngine/Renderer/Core/Texture.h"
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include "StrikeEngine/Scene/Components/TransformComponents.h"
#include <StrikeEngine/Scene/Components/ModelComponent.h>
#include "StrikeEngine/Renderer/Core/Skybox.h"
#include "StrikeEngine/Scene/RenderCommand.h"
#include <queue>

namespace StrikeEngine {


    class Renderer {
    public:
        static void Create();
        static Renderer* Get();
        static void Destroy();
        void Init();

        void BeginScene(CameraComponent* camera);
        void EndScene();
        void SubmitScene(Scene* scene);
        void SubmitSkybox(Skybox* skybox);
        void Render();
        void RenderShadowMaps();

        void SubmitCommand(RenderCommand renderCommand);

        void SetDefaultTexture(const std::string& path);
        friend class LightManager;
    private:
        Renderer();
        ~Renderer();

        void RenderSkybox(glm::mat4 cameraView, glm::mat4 cameraProjection);
        void SubmitEntity(Entity entity, const glm::mat4& transformationMatrix);
        void BindShaderMVP(Shader* shader, const RenderCommand& command);
        void BindShaderMaterials(Shader* shader, ModelPart* part);
        void RenderModelParts(Shader* shader, const RenderCommand& command);
        void BindTextures(ModelPart* part);
        void UnbindTextures(ModelPart* part);

        void CreateFramebuffer();
        // Full-screen quad setup
        void SetupFullScreenQuad();
        void RenderFullScreenQuad();

        void RenderShadowMapTexture();

        static Renderer* s_Instance;
        std::queue<RenderCommand> m_RenderQueue;
        Skybox* m_Skybox;
        bool m_RenderSkybox;
        Texture* m_DefaultTexture;

        GLuint m_Framebuffer;
        GLuint m_ColorAttachment;
        GLuint m_DepthAttachment;
        GLuint m_Width;
        GLuint m_Height;

        GLuint m_QuadVAO, m_QuadVBO;
        Shader* m_FullScreenQuadShader;

        glm::mat4 m_CameraViewProjectionMatrix;
        glm::mat4 m_CameraViewMatrix;
        glm::mat4 m_CameraProjectionMatrix;
        glm::vec3 m_CameraPosition;
    };
}
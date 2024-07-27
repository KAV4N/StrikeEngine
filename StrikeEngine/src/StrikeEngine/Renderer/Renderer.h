#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include "StrikeEngine/Scene/Camera.h"
#include "StrikeEngine/Renderer/Shader.h"
#include "StrikeEngine/Renderer/Model.h"
#include "StrikeEngine/Renderer/Texture.h"
#include "StrikeEngine/Scene/Light.h"
#include <StrikeEngine/Scene/Entity.h>
#include "Skybox.h"

namespace StrikeEngine {

    struct RenderCommand {
        glm::mat4 transformationMatrix;
        Model* model;
        glm::vec3 cameraPosition;
    };

    class Renderer {
    public:
        static void Create();
        static Renderer* Get();
        static void Destroy();

        static void Init();

        void BeginScene(Camera* camera);
        void EndScene();
        void SubmitScene(const std::vector<Entity*>& entities, const glm::vec3& cameraPosition);
        void Submit(const glm::mat4& transformationMatrix, Model* model, const glm::vec3& cameraPosition);
        void SubmitSkybox(Skybox* skybox);

        void SetDefaultTexture(const std::string& path);

    private:
        Renderer();
        ~Renderer();

        void Render();
        void RenderSkybox();

        void BindShaderMVP(Shader* shader, const RenderCommand& command);
        void BindShaderMaterials(Shader* shader, ModelPart* part);


        void RenderModelParts(Shader* shader, const RenderCommand& command);
        void BindTextures(ModelPart* part);
        void UnbindTextures(ModelPart* part);

    private:
        static Renderer* s_Instance;
        glm::mat4 m_ViewProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionMatrix;
        Skybox* m_Skybox;
        Texture* m_DefaultTexture = nullptr;

        std::unordered_map<Shader*, std::vector<RenderCommand>> m_RenderQueue;
        bool m_RenderSkybox = false;

    };
}

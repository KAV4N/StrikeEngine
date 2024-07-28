#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <entt/entt.hpp>
#include "StrikeEngine/Scene/Camera.h"
#include "StrikeEngine/Renderer/Shader.h"
#include "StrikeEngine/Renderer/Model.h"
#include "StrikeEngine/Renderer/Texture.h"
#include "StrikeEngine/Scene/Light.h"
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include "StrikeEngine/Scene/Components/TransformComponents.h"
#include "Skybox.h"

namespace StrikeEngine {

    struct RenderCommand {
        glm::mat4 transformationMatrix;
        Entity entity;
        glm::vec3 cameraPosition;
    };

    class Renderer {
    public:
        static void Create();
        static Renderer* Get();
        static void Destroy();
        static void Init();
        void BeginScene(CameraComponent* camera);
        void EndScene();
        void SubmitScene(Scene* scene);
        void SubmitEntity(Entity entity, const glm::mat4& transformationMatrix);
        void SubmitSkybox(Skybox* skybox);
        void Render();
        void SetDefaultTexture(const std::string& path);

    private:
        Renderer();
        ~Renderer();
        void RenderSkybox();
        void BindShaderMVP(Shader* shader, const RenderCommand& command);
        void BindShaderMaterials(Shader* shader, ModelPart* part);
        void RenderModelParts(Shader* shader, const RenderCommand& command);
        void BindTextures(ModelPart* part);
        void UnbindTextures(ModelPart* part);
    private:
        static Renderer* s_Instance;

        glm::mat4 m_CameraViewProjectionMatrix;
        glm::mat4 m_CameraViewMatrix;
        glm::mat4 m_CameraProjectionMatrix;
        glm::vec3 m_CameraPosition;

        Texture* m_DefaultTexture;
        Skybox* m_Skybox;
        bool m_RenderSkybox;
        std::unordered_map<Shader*, std::vector<RenderCommand>> m_RenderQueue;
    };
}

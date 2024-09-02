#pragma once

#include <vector>
#include <memory>
#include <string>
#include <entt/entt.hpp>
#include "Entity.h"
#include "StrikeEngine/Renderer/Core/Shader.h"
#include "StrikeEngine/Renderer/Core/Model.h"
#include "StrikeEngine/Renderer/Core/Skybox.h"
#include "StrikeEngine/Renderer/Managers/LightManager.h"

#include "StrikeEngine/Scene/Components/ModelComponent.h"
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include "StrikeEngine/Scene/Components/TransformComponents.h"
#include "StrikeEngine/Scene/Components/LightComponents.h"
#include "StrikeEngine/Scene/RenderCommand.h"


namespace StrikeEngine {

    class Scene {
    public:

        Scene();
        ~Scene();
        
        RenderCommand BuildRenderCommand() const;

        std::vector<Entity> GetDirectionalLights() const;

        std::vector<Entity> GetPointLights() const;

        std::vector<Entity> GetSpotLights() const;

        std::vector<Entity> GetShadowCastingLights() const;


        Entity CreateEntity(Model* model, const std::string& name = std::string());
        std::vector<Entity> GetAllEntitiesWithModelComponent() const;
        Entity CreateShadowCaster(Entity lightEntity);


        void RemoveEntity(entt::entity entity);

        void Setup();
        void SetCamera(entt::entity cameraEntity);

        void OnUpdate(float deltaTime);
        void Render();

        Entity GetCameraEntity() const;

        inline const entt::registry& GetRegistry() const { return m_Registry; }

        friend class LightManager;
        friend class TransformSystem;
        friend class Entity;

    private:
        Entity CreateDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);
        Entity CreatePointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius);
        Entity CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec3& color, float intensity);



    private:
        entt::registry m_Registry;

        Entity m_CameraEntity;

        std::unique_ptr<Skybox> m_Skybox;
    };

}

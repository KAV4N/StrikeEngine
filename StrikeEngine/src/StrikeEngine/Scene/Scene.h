#pragma once

#include <vector>
#include <memory>
#include <string>
#include <entt/entt.hpp>
#include "Entity.h"
#include "StrikeEngine/Renderer/Shader.h"
#include "StrikeEngine/Renderer/Model.h"
#include "StrikeEngine/Scene/Light.h"
#include "StrikeEngine/Renderer/Skybox.h"
#include "StrikeEngine/Renderer/LightManager.h"

#include "StrikeEngine/Scene/Components/ModelComponent.h"
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include "StrikeEngine/Scene/Components/TransformComponents.h"
#include "StrikeEngine/Scene/Components/LightComponent.h"

namespace StrikeEngine {

    class Scene {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(Model* model, const std::string& name = std::string());

        void RemoveEntity(entt::entity entity);

        void Setup();
        void SetCamera(entt::entity cameraEntity);

        void Update();
        void Render();

        Entity GetCameraEntity() const;
        std::vector<DirectionalLight>& GetDirectionalLights();
        std::vector<PointLight>& GetPointLights();
        std::vector<SpotLight>& GetSpotLights();

        inline const entt::registry& GetRegistry() const { return m_Registry; }

        friend class LightManager;
        friend class TransformSystem;
        friend class Entity;

    private:

        

        void AddDirectionalLight(const DirectionalLight& light);
        void RemoveDirectionalLight(size_t index);
        void UpdateDirectionalLight(size_t index, const DirectionalLight& light);

        void AddPointLight(const PointLight& light);
        void RemovePointLight(size_t index);
        void UpdatePointLight(size_t index, const PointLight& light);

        void AddSpotLight(const SpotLight& light);
        void RemoveSpotLight(size_t index);
        void UpdateSpotLight(size_t index, const SpotLight& light);

        void ClearDirectionalLights();
        void ClearPointLights();
        void ClearSpotLights();

    private:
        entt::registry m_Registry;
        entt::entity m_CameraEntity{ entt::null };
        std::unique_ptr<Skybox> m_Skybox;

        // Light management
        std::vector<DirectionalLight> m_DirectionalLights;
        std::vector<PointLight> m_PointLights;
        std::vector<SpotLight> m_SpotLights;
    };

}

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
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include "StrikeEngine/Scene/Components/TransformComponents.h"

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
        const std::vector<Light>& GetLights() const;
        inline const entt::registry& GetRegistry() const { return m_Registry; }

        friend class LightManager;
        friend class TransformSystem;
        friend class Entity;

    private:
        void AddLight(const Light& light);
        void RemoveLight(size_t index);
        void UpdateLight(size_t index, const Light& light);
        void ClearLights();

    private:
        entt::registry m_Registry;
        entt::entity m_CameraEntity{ entt::null };
        std::vector<Light> m_Lights;
        std::unique_ptr<Skybox> m_Skybox;
    };

}

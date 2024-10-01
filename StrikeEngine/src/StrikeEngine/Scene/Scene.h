#pragma once

#include <vector>
#include <memory>
#include <string>
#include <entt/entt.hpp>
#include "StrikeEngine/Events/Event.h"
#include "Entity.h"
#include "StrikeEngine/Graphics/Core/Shader.h"
#include "StrikeEngine/Graphics/Core/Model.h"
#include "StrikeEngine/Graphics/Core/Skybox.h"
#include "StrikeEngine/Graphics/Managers/LightManager.h"

#include "StrikeEngine/Scene/Components/ModelComponent.h"
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include "StrikeEngine/Scene/Components/TransformComponent.h"
#include "StrikeEngine/Scene/Components/LightComponents.h"
#include "StrikeEngine/Scene/RenderCommand.h"
#include <StrikeEngine/Core/LayerStack.h>
#include "StrikeEngine/Scene/UI/ScreenPanel.h"


namespace StrikeEngine {

    class Scene {
    public:
        friend class World;
        friend class LightManager;
        friend class TransformSystem;
        friend class Entity;

        Scene();
        ~Scene();

        std::vector<Entity> GetDirectionalLights() const;
        std::vector<Entity> GetPointLights() const;
        std::vector<Entity> GetSpotLights() const;
        std::vector<Entity> GetShadowCastingLights() const;


        Entity CreateEntity(Model* model, const std::string& name = std::string());
        Entity CreateShadowCaster(Entity lightEntity);

        void RemoveEntity(entt::entity entity);

        void OnEvent(Event& event);
        void OnUpdate(float deltaTime);

        inline const entt::registry& GetRegistry() const { return m_Registry; }
        inline ScreenPanel* GetActiveView() { return m_ActiveView; }
    protected:

    private:
        Entity CreateDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);
        Entity CreatePointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius);
        Entity CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec3& color, float intensity);

        void RenderScene(FrameBuffer* frameBuffer);
        void SubmitMesh();
        void SubmitSkybox();

        void SubmitModelMesh(const std::vector<Entity>& modelChildren, const glm::mat4& rootTransform);
        void SubmitLight(const Entity& entity, const glm::mat4& worldTransform);



    private:
        entt::registry m_Registry;
        std::unique_ptr<Skybox> m_Skybox;
        std::vector<ScreenPanel*> m_ViewList;
        ScreenPanel* m_ActiveView;
    };

}

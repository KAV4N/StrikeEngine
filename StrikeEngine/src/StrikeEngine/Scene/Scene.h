#pragma once
#include <entt/entt.hpp>
#include <memory>
#include <vector>
#include "Entity.h"
#include "SceneGraph.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Graphics/Core/Skybox.h"
#include "StrikeEngine/Graphics/Core/Model.h"
#include "UI/ScreenPanel.h"
#include "Components/TransformComponent.h"

namespace StrikeEngine {

    class Scene {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity();
        Entity CreateEmptyEntity();
        Entity CreateRenderableEntity(Model* model, const std::string& name = "");
        void DestroyEntity(Entity entity);

        Entity CreateDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);
        Entity CreatePointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius);
        Entity CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec3& color, float intensity);
        Entity CreateShadowCaster(Entity lightEntity);

  
        std::vector<Entity> GetDirectionalLights() const;
        std::vector<Entity> GetPointLights() const;
        std::vector<Entity> GetSpotLights() const;
        std::vector<Entity> GetShadowCastingLights() const;


        void OnUpdate(float t_DeltaTime);
        void OnEvent(Event& event);
        void RenderScene(FrameBuffer* frameBuffer);


        ScreenPanel* GetActiveView() const { return m_ActiveView; }
        std::vector<ScreenPanel*>& GetViewList() { return m_ViewList; }
        Skybox* GetSkybox() const { return m_Skybox.get(); }

        SceneGraph& GetSceneGraph() { return m_SceneGraph; }
        const SceneGraph& GetSceneGraph() const { return m_SceneGraph; }

        template<typename... T>
        auto GetAllEntitiesWith() {
            return m_Registry.view<T...>();
        }

    private:
        void UpdateNodeTransforms(SceneNode* node, const glm::mat4& parentTransform);
        void SubmitMesh(SceneNode* node, const glm::mat4& parentTransform);
        void SubmitSkybox();
        entt::registry& GetRegistry() { return m_Registry; }
    private:
        entt::registry m_Registry;
        SceneGraph m_SceneGraph;

        ScreenPanel* m_ActiveView;
        std::vector<ScreenPanel*> m_ViewList;
        std::unique_ptr<Skybox> m_Skybox;

        friend class Entity;
        friend class SceneGraph;
        friend class LightManager;
    };

}
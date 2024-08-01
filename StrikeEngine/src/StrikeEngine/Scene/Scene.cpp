#include "strikepch.h"
#include "Scene.h"
#include "StrikeEngine/Renderer/Renderer.h"
#include "StrikeEngine/Scene/Systems/TransformSystem.h"
#include "Components/ModelComponent.h"
#include "Components/ShadowCasterComponent.h"
#include "Components/LightComponents.h"

namespace StrikeEngine {

    Scene::Scene() {
        m_Skybox = std::make_unique<Skybox>();

        m_CameraEntity = m_Registry.create();
        auto& camera = m_Registry.emplace<CameraComponent>(m_CameraEntity, 70.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
        camera.Position = glm::vec3(0.0f, 3.0f, 6.0f);
        camera.UpdateViewMatrix();
    }

    Scene::~Scene() {
        // Cleanup resources if needed
    }



    std::vector<Entity>  Scene::GetDirectionalLights() const {
        std::vector<Entity> lights;
        auto view = m_Registry.view<DirectionalLightComponent>();
        for (auto entity : view) {
            lights.emplace_back(entity, const_cast<Scene*>(this));
            
        }
        return lights;
    }

    std::vector<Entity>  Scene::GetPointLights() const {
        std::vector<Entity> lights;
        auto view = m_Registry.view<PointLightComponent>();
        for (auto entity : view) {
            lights.emplace_back(entity, const_cast<Scene*>(this));
        }
        return lights;
    }

    std::vector<Entity>  Scene::GetSpotLights() const {
        std::vector<Entity> lights;
        auto view = m_Registry.view<SpotLightComponent>();
        for (auto entity : view) {
            lights.emplace_back(entity, const_cast<Scene*>(this));    
        }
        return lights;
    }

    std::vector<Entity> Scene::GetShadowCastingLights() const {
        std::vector<Entity> shadowCasters;
        auto view = m_Registry.view<ShadowCasterComponent>();
        for (auto entity : view) {
            shadowCasters.emplace_back(entity, const_cast<Scene*>(this));
        }
        return shadowCasters;
    }


    Entity Scene::CreateEntity(Model* model, const std::string& name) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<TransformComponent>();
        entity.AddComponent<ModelComponent>(model);
        entity.AddComponent<PositionComponent>();
        entity.AddComponent<RotationComponent>();
        entity.AddComponent<ScaleComponent>();

        return entity;
    }

    void Scene::RemoveEntity(entt::entity entity) {
        m_Registry.destroy(entity);
    }

    Entity Scene::CreateDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<DirectionalLightComponent>(direction, color, intensity);
        return entity;
    }

    Entity Scene::CreatePointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<PointLightComponent>(position, color, intensity, radius);
        return entity;
    }

    Entity Scene::CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, float cutoff, const glm::vec3& color, float intensity) {
        Entity entity(m_Registry.create(), this);
        entity.AddComponent<SpotLightComponent>(position, direction, cutoff, color, intensity);
        return entity;
    }

    Entity Scene::CreateShadowCaster(Entity lightEntity)
    {
        if (!lightEntity.HasComponent<DirectionalLightComponent>() &&
            !lightEntity.HasComponent<PointLightComponent>() &&
            !lightEntity.HasComponent<SpotLightComponent>()) {
            throw std::runtime_error("Entity must have a light component to create a ShadowCasterComponent.");
        }

        lightEntity.AddComponent<ShadowCasterComponent>();
        return lightEntity;
    }


    void Scene::Setup() {
        // Scene setup code
    }

    void Scene::Update() {
        TransformSystem::Update(this);
    }

    void Scene::SetCamera(entt::entity cameraEntity) {
        m_CameraEntity = cameraEntity;
    }

    void Scene::Render() {
        Update();

        Renderer* renderer = Renderer::Get();
        auto& camera = m_Registry.get<CameraComponent>(m_CameraEntity);

        renderer->BeginScene(&camera);

        renderer->SubmitSkybox(m_Skybox.get());
        renderer->SubmitScene(this);

        renderer->EndScene();
    }

    Entity Scene::GetCameraEntity() const {
        return Entity{ m_CameraEntity, const_cast<Scene*>(this) };
    }

}

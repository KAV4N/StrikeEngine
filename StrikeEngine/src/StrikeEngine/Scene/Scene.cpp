#include "strikepch.h"
#include "Scene.h"
#include "StrikeEngine/Renderer/Renderer.h"
#include "StrikeEngine/Scene/Systems/TransformSystem.h"
#include "Components/ModelComponent.h"

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

    void Scene::AddDirectionalLight(const DirectionalLight& light) {
        m_DirectionalLights.push_back(light);
    }

    void Scene::RemoveDirectionalLight(size_t index) {
        if (index < m_DirectionalLights.size()) {
            m_DirectionalLights.erase(m_DirectionalLights.begin() + index);
        }
    }

    void Scene::UpdateDirectionalLight(size_t index, const DirectionalLight& light) {
        if (index < m_DirectionalLights.size()) {
            m_DirectionalLights[index] = light;
        }
    }

    void Scene::AddPointLight(const PointLight& light) {
        m_PointLights.push_back(light);
    }

    void Scene::RemovePointLight(size_t index) {
        if (index < m_PointLights.size()) {
            m_PointLights.erase(m_PointLights.begin() + index);
        }
    }

    void Scene::UpdatePointLight(size_t index, const PointLight& light) {
        if (index < m_PointLights.size()) {
            m_PointLights[index] = light;
        }
    }

    void Scene::AddSpotLight(const SpotLight& light) {
        m_SpotLights.push_back(light);
    }

    void Scene::RemoveSpotLight(size_t index) {
        if (index < m_SpotLights.size()) {
            m_SpotLights.erase(m_SpotLights.begin() + index);
        }
    }

    void Scene::UpdateSpotLight(size_t index, const SpotLight& light) {
        if (index < m_SpotLights.size()) {
            m_SpotLights[index] = light;
        }
    }

    void Scene::ClearDirectionalLights() {
        m_DirectionalLights.clear();
    }

    void Scene::ClearPointLights() {
        m_PointLights.clear();
    }

    void Scene::ClearSpotLights() {
        m_SpotLights.clear();
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

    std::vector<DirectionalLight>& Scene::GetDirectionalLights() {
        return m_DirectionalLights;
    }

    std::vector<PointLight>& Scene::GetPointLights() {
        return m_PointLights;
    }

    std::vector<SpotLight>& Scene::GetSpotLights() {
        return m_SpotLights;
    }

}

#include "strikepch.h"
#include "Scene.h"
#include "StrikeEngine/Renderer/Renderer.h"
#include "StrikeEngine/Renderer/LightManager.h"
#include "StrikeEngine/Scene/Systems/TransformSystem.h"

namespace StrikeEngine {
    Scene::Scene() {
        m_Skybox = std::make_unique<Skybox>();

        m_CameraEntity = m_Registry.create();
        auto& camera = m_Registry.emplace<CameraComponent>(m_CameraEntity, 70.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
        camera.Position = glm::vec3(0.0f, 3.0f, 6.0f);
        camera.UpdateViewMatrix();
    }

    Scene::~Scene() {
  
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

    void Scene::AddLight(const Light& light) {
        m_Lights.push_back(light);
    }

    void Scene::RemoveLight(size_t index) {
        if (index < m_Lights.size()) {
            m_Lights.erase(m_Lights.begin() + index);
        }
    }

    void Scene::UpdateLight(size_t index, const Light& light) {
        if (index < m_Lights.size()) {
            m_Lights[index] = light;
        }
    }

    void Scene::ClearLights() {
        m_Lights.clear();
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

        renderer->SubmitScene(m_Registry);

        renderer->EndScene();
    }


    Entity Scene::GetCameraEntity() const {
        return Entity{ m_CameraEntity, const_cast<Scene*>(this) };
    }

    const std::vector<Light>& Scene::GetLights() const {
        return m_Lights;
    }
}

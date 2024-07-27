#include "strikepch.h"
#include "Scene.h"
#include "StrikeEngine/Renderer/Renderer.h"
#include <StrikeEngine/Renderer/LightManager.h>

namespace StrikeEngine {
    Scene::Scene() {
        m_Camera = new Camera(70.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
        m_Camera->SetPosition(glm::vec3(0.0f, 3.f, 6.f));
        m_Skybox = new Skybox();
    }

    Scene::~Scene() {
        for (auto entity : m_Entities) {
            delete entity;
        }
        delete m_Camera;
    }

    Entity* Scene::CreateEntity(Model* model, const glm::vec3& position,
        const glm::vec3& rotation, const glm::vec3& scale) {
        Entity* entity = new Entity(model, position, rotation, scale);
        AddEntity(entity);
        return entity;
    }

    void Scene::AddEntity(Entity* entity) {
        m_Entities.push_back(entity);
    }

    void Scene::AddLight(const Light& light) {
        m_Lights.push_back(light);
    }

    void Scene::RemoveLight(size_t index)
    {
    }

    void Scene::UpdateLight(size_t index, const Light& light)
    {
    }

    void Scene::ClearLights()
    {

    }

    void Scene::Setup() {
    }



    void Scene::Update() {

    }

    void Scene::SetCamera(Camera* camera) {
        m_Camera = camera;
    }

    void Scene::Render() {
        Renderer* renderer = Renderer::Get();
        renderer->BeginScene(m_Camera);

        renderer->SubmitSkybox(m_Skybox);
        renderer->SubmitScene(m_Entities, m_Camera->GetPosition());

        renderer->EndScene();
    }

    Camera* Scene::GetCamera() {
        return m_Camera;
    }

    const std::vector<Light>& Scene::GetLights() {
        return m_Lights;
    }
}

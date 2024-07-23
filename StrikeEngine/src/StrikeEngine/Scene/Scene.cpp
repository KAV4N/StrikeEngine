#include "strikepch.h"
#include "Scene.h"
#include "StrikeEngine/Renderer/Renderer.h"

namespace StrikeEngine {
    Scene::Scene() {
        m_Camera = new Camera(70.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
        //m_Camera->MoveRight(1);
        m_Camera->SetPosition(glm::vec3(0.0f, 1.f, 3.f));
        m_Skybox = new Skybox();
    }

    Scene::~Scene() {
        for (auto entity : m_Entities) {
            delete entity;
        }
        delete m_Camera;
    }

    void Scene::AddEntity(Entity* entity) {
        m_Entities.push_back(entity);
    }

    void Scene::AddLight(Light light) {
        m_Lights.push_back(light);
    }

    void Scene::SetCamera(Camera* camera) {
        m_Camera = camera;
    }


    void Scene::Update() {
        // Update entities and other scene elements
    }

    void Scene::Render() {
        Renderer* renderer = Renderer::Get();
        renderer->BeginScene(m_Camera);

        //renderer->SubmitSkybox(m_Skybox, m_Camera->GetPosition(), m_Camera->GetOrientation(), m_Camera->GetUp());
        renderer->SubmitSkybox(m_Skybox);
        renderer->SubmitScene(m_Entities, m_Lights, m_Camera->GetPosition());

        renderer->EndScene();
    }


    Camera* Scene::GetCamera() {
        return m_Camera;
    }


}

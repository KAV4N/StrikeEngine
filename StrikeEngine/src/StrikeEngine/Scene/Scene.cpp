#include "strikepch.h"
#include "Scene.h"
#include "StrikeEngine/Renderer/Renderer.h"

namespace StrikeEngine {
    Scene::Scene() {
        m_Camera = new Camera(70.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
        // ONLY FOR TESTING
        m_Camera->SetPosition(glm::vec3(0.0f, 1.f, 3.f));
       // m_Camera->RotatePitch(-15.f);
    }

    Scene::~Scene() {
        for (auto entity : m_Entities) {
            delete entity;
        }
        for (auto light : m_Lights) {
            delete light;
        }
        delete m_Camera;
    }

    void Scene::AddEntity(Entity* entity) {
        m_Entities.push_back(entity);
    }

    void Scene::AddLight(Light* light) {
        m_Lights.push_back(light);
    }

    void Scene::SetCamera(Camera* camera) {
        m_Camera = camera;
    }

    void Scene::Update() {
        /*
        for (auto entity : m_Entities) {
            entity->Update();
        }
        */
    }

    void Scene::Render(Shader* shader) {
        Renderer* renderer = Renderer::Get();
        renderer->BeginScene(m_Camera);

        for (auto entity : m_Entities) {
            entity->IncreaseRotation(glm::vec3(0.f, 0.f, 1.f)); // TODO: REMOVE, ONLY FOR TESTING
            renderer->Update(entity->GetTransformationMatrix(), shader, m_Lights, m_Camera->GetPosition());
            renderer->Render(entity->GetModel());
        }

        renderer->EndScene();
    }


    Camera* Scene::GetCamera() {
        return m_Camera;
    }
}

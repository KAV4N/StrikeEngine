#include "strikepch.h"
#include "Scene.h"
#include "StrikeEngine/Renderer/Renderer.h"

namespace StrikeEngine {
    Scene::Scene()
    {
        m_Camera = new Camera(70.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
        m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 10.f));
        m_Camera->RotatePitch(15);
       
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

    void Scene::SetCamera(Camera* camera) {
        m_Camera = camera;
    }

    void Scene::Update() {
        /*
        for (auto entity : m_Entities) {
            entity->Update();
        }*/
    }

    void Scene::Render(Shader* shader) {
        for (auto entity : m_Entities) {
            entity->IncreaseRotation(glm::vec3(0.f, 1.f, 0.f)); // TODO: REMOVE, ONLY FOR TESTING
            Renderer::Get()->Update(entity->GetTransformationMatrix(), shader);
            Renderer::Get()->Render(entity->GetModel());
        }
    }

    Camera* Scene::GetCamera() {
        return m_Camera;
    }
}

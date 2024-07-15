#include "strikepch.h"
#include "Scene.h"
#include "StrikeEngine/Renderer/Renderer.h"

namespace StrikeEngine {
    Scene::Scene() : m_Camera(nullptr) {}

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
            Renderer::Get()->Update(entity, shader);
        }
    }

    Camera* Scene::GetCamera() const {
        return m_Camera;
    }
}

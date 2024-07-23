#include "strikepch.h"
#include "World.h"

namespace StrikeEngine {
    World::World() : m_ActiveScene(nullptr) {}

    World::~World() {
        for (auto scene : m_Scenes) {
            delete scene;
        }
    }

    void World::AddScene(Scene* scene) {
        m_Scenes.push_back(scene);
        if (m_ActiveScene == nullptr) {
            m_ActiveScene = scene;
        }
    }

    void World::SetActiveScene(int index) {
        if (index >= 0 && index < m_Scenes.size()) {
            m_ActiveScene = m_Scenes[index];
        }
    }

    void World::Update() {
        if (m_ActiveScene) {
            m_ActiveScene->Update();
        }
    }

    void World::Render() {
        if (m_ActiveScene) {
            m_ActiveScene->Render();
        }
    }

    Scene* World::GetActiveScene() {
        return m_ActiveScene;
    }
}

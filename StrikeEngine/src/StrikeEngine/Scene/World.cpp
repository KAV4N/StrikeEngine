#include "strikepch.h"
#include "World.h"
#include "StrikeEngine/Scene/Systems/TransformSystem.h"
#include "StrikeEngine/Graphics/Renderer/Renderer.h"

namespace StrikeEngine {

    World* World::s_Instance = nullptr;

    World::World(GLuint resX, GLuint resY) 
        : m_ActiveScene(nullptr),
        m_FrameBuffer(new FrameBuffer(resX, resY))
    
    {
        Renderer::Create();
        Renderer::Get()->Init();
        
    }

    World::~World() {
        for (auto scene : m_Scenes) {
            delete scene;
        }
        delete m_FrameBuffer;
    }

    void World::Create() {
        if (s_Instance == nullptr) {
            s_Instance = new World();
        }
    }


    World* World::Get() {
    
        return s_Instance;
    }

    void World::AddScene(Scene* scene) {
        m_Scenes.push_back(scene);
        if (m_ActiveScene == nullptr) {
            m_ActiveScene = scene;
        }
    }

    void World::AddScene() {
        Scene* scene = new Scene();
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



    void World::OnUpdate(float deltaTime) {
        m_ActiveScene->OnUpdate(deltaTime);
    }


    void World::OnRender() {
        if (m_ActiveScene) {
            
            m_ActiveScene->RenderScene(m_FrameBuffer);
            Renderer::Resize(m_Width, m_Height);
            Renderer::BindDefaultFrameBuffer();
            Renderer::Get()->DrawTexturedQuad(glm::vec2(0.0f, 0.0f), glm::vec2(1.f, 1.f), m_FrameBuffer->GetColorAttachment());
        }

    }

    void World::OnEvent(Event& event)
    {
        m_ActiveScene->OnEvent(event);
    }

    Scene* World::GetActiveScene() {
        return s_Instance->m_ActiveScene;
    }

}

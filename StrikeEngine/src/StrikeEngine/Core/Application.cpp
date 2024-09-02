#include "strikepch.h"
#include "Application.h"

#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Renderer/Renderer3D/Renderer.h"
#include "StrikeEngine/Renderer/Managers/ModelManager.h"
#include "StrikeEngine/Renderer/Managers/ShaderManager.h"
#include "StrikeEngine/Renderer/Managers/LightManager.h"

#include "Input.h"
#include <glad/glad.h>
#include <StrikeEngine/Scene/World.h>

//TODO: REMOVE AFTER TESTING
#include <StrikeEngine/Scene/Systems/TransformSystem.h> 
#include <chrono>
#include <imgui.h>
//---------------------------------------------

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

namespace StrikeEngine
{
    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        STRIKE_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        World::Create();
        Renderer::Create();
        Renderer::Get()->Init();
        Renderer::Get()->SetDefaultTexture(DEFAULT_TEXTURE);

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

        Scene* scene = new Scene();
        World::Get()->AddScene(scene);

        // Load models
        Model* model = ModelManager::Get()->LoadModel("assets/objects/panzer/14077_WWII_Tank_Germany_Panzer_III_v1_L2.obj");
        //model->SetShader(ShaderManager::Get()->GetShader("ShinyShader"));

        Model* model2 = ModelManager::Get()->LoadModel("assets/objects/penguin/PenguinBaseMesh.obj");
        //model2->SetShader(ShaderManager::Get()->GetShader("ComicShader"));

        Model* model3 = ModelManager::Get()->LoadModel("assets/objects/box/circle.obj");
        //model3->SetShader(ShaderManager::Get()->GetShader("ShinyShader"));

        Entity entity = World::Get()->GetActiveScene()->CreateEntity(model);
        TransformSystem::IncreaseRotation(entity, glm::uvec3(270.f, 0.f, 45.f));
        TransformSystem::SetPosition(entity, glm::uvec3(1.f, 0.f, 0.f));

        Entity entity2 = World::Get()->GetActiveScene()->CreateEntity(model2);
        TransformSystem::SetPosition(entity2, glm::uvec3(5.f, 0.f, 0.f));

        Entity entity3 = World::Get()->GetActiveScene()->CreateEntity(model3);
        TransformSystem::SetScale(entity3, glm::vec3(10.f));

        Entity light = LightManager::Get()->CreateSpotLight(
            glm::vec3(2.0f, 2.0f, 1.0f),
            glm::vec3(-0.5f, 0.2f, -0.5f),
            cos(glm::radians(60.0f)),
            glm::vec3(1.0f, 1.0f, 1.0f),
            2.0f);
        LightManager::Get()->CreateShadowCaster(light);

        Entity light1 = LightManager::Get()->CreateSpotLight(
            glm::vec3(0.0f, 2.0f, 1.0f),
            glm::vec3(0.0f, 0.2f, -1.f),
            cos(glm::radians(60.f)),
            glm::vec3(1.0f, 1.0f, 1.0f),
            1.0f);
        LightManager::Get()->CreateShadowCaster(light1);

        Entity light2 = LightManager::Get()->CreateDirectionalLight(
            glm::vec3(0.f, -4.f, -6.f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            0.8f);
        LightManager::Get()->CreateShadowCaster(light2);
    }

    Application::~Application()
    {
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
                break;
        }
    }

    void Application::OnUpdate() {
        /*
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        */
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTimeDuration = now - m_LastFrameTime;
        m_LastFrameTime = now;
        float deltaTime = deltaTimeDuration.count();


        for (Layer* layer : m_LayerStack)
            layer->OnUpdate(deltaTime);

        //m_ImGuiLayer->Begin();

        //for (Layer* layer : m_LayerStack)
        //    layer->OnImGuiRender();

        //m_ImGuiLayer->End();

        World::Get()->Render();
        Renderer::Get()->Render();

        m_Window->OnUpdate();
    }

    void Application::Run() {
        m_LastFrameTime = std::chrono::high_resolution_clock::now();
        while (m_Running) {
            OnUpdate();
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        return true;
    }
}

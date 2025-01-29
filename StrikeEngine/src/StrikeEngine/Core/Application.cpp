#include "strikepch.h"
#include "Application.h"

#include "StrikeEngine/Core/Log.h"

#include "StrikeEngine/Graphics/Managers/ShaderManager.h"
#include "StrikeEngine/Graphics/Managers/MaterialManager.h"
#include "StrikeEngine/Graphics/Managers/TextureManager.h"
#include "StrikeEngine/Graphics/Managers/MeshManager.h"

#include "Input.h"
#include <glad/glad.h>


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

        CreateManagers();

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
    }

    void Application::CreateManagers() {
        ShaderManager::Create();
        TextureManager::Create();
        MaterialManager::Create();
        MeshManager::Create();
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
        
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTimeDuration = now - m_LastFrameTime;
        m_LastFrameTime = now;
        float deltaTime = deltaTimeDuration.count();

        for (Layer* layer : m_LayerStack)
            layer->OnUpdate(deltaTime);


        m_ImGuiLayer->Begin();
        for (Layer* layer : m_LayerStack) {
            layer->OnRender();
            layer->OnImGuiRender();
        }
        m_ImGuiLayer->End();

        m_Window->OnUpdate();
    }

    void Application::Run() {
        //Renderer::Get()->Resize(m_Window->GetWidth(), m_Window->GetHeight());
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

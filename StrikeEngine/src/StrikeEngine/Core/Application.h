#pragma once

#include "Core.h"

#include "Window.h"
#include "StrikeEngine/Core/LayerStack.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/ApplicationEvent.h"

#include "StrikeEngine/ImGui/ImGuiLayer.h"



namespace StrikeEngine
{


    class STRIKE_API Application
    {
    public:
        friend class Renderer;
        Application();
        virtual ~Application();

        void Run();
        void OnEvent(Event& e);

        void OnUpdate();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        inline Window& GetWindow() { return *m_Window; }

        inline static Application& Get() { return *s_Instance; }

    private:
        bool OnWindowClose(WindowCloseEvent& e);

        bool OnWindowResize(WindowResizeEvent& e);

        std::unique_ptr<Window> m_Window;
        ImGuiLayer* m_ImGuiLayer;
        bool m_Running = true;
        LayerStack<Layer*> m_LayerStack;
        std::chrono::high_resolution_clock::time_point m_LastFrameTime;

    private:
        static Application* s_Instance;
    };

    // To be defined in CLIENT
    Application* CreateApplication();
}


#pragma once

#include "Core.h"

#include "Window.h"
#include "StrikeEngine/Core/LayerStack.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/ApplicationEvent.h"




#include "StrikeEngine/ImGui/ImGuiLayer.h"



namespace StrikeEngine
{

    class World;

    class STRIKE_API Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();
        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        inline Window& GetWindow() { return *m_Window; }

        inline static Application& Get() { return *s_Instance; }
    private:
        bool OnWindowClose(WindowCloseEvent& e);

        std::unique_ptr<Window> m_Window;
        ImGuiLayer* m_ImGuiLayer;
        bool m_Running = true;
        LayerStack m_LayerStack;

        World* m_World;

    private:
        static Application* s_Instance;
    };

    // To be defined in CLIENT
    Application* CreateApplication();
}


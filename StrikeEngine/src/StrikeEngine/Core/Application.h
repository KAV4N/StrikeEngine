#pragma once

#include "Core.h"
#include "Window.h"
#include "StrikeEngine/Core/LayerStack.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/ApplicationEvent.h"
#include "StrikeEngine/ImGui/ImGuiLayer.h"

namespace StrikeEngine {

    class Application {
    public:
        friend class Renderer;
        Application();
        virtual ~Application();

        void run();
        void onEvent(Event& e);
        void onUpdate();

        void pushLayer(Layer* layer);
        void pushOverlay(Layer* overlay);

        inline Window& getWindow() { return *mWindow; }
        inline static Application& get() { return *sInstance; }

    private:
        bool onWindowClose(WindowCloseEvent& e);
        bool onWindowResize(WindowResizeEvent& e);
        void createManagers();

        std::unique_ptr<Window> mWindow;
        ImGuiLayer* mImGuiLayer;
        bool mRunning = true;
        LayerStack<Layer*> mLayerStack;
        std::chrono::high_resolution_clock::time_point mLastFrameTime;

    private:
        static Application* sInstance;
    };

    Application* createApplication();
}
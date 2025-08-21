#pragma once
#include "Core.h"
#include "Window.h"
#include "Timer.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/ApplicationEvent.h"
#include "StrikeEngine/ImGui/ImGuiLayer.h"
#include "StrikeEngine/Scene/World.h"

namespace StrikeEngine {
    class Application {
    public:
        friend class Renderer;
        Application();
        virtual ~Application();
        void run();
        void onEvent(Event& e);
        void onUpdate();

        // Timer access methods
        void setTargetFPS(float fps) { mTimer.setTargetFPS(fps); }
        float getTargetFPS() const { return mTimer.getTargetFPS(); }
        float getCurrentFPS() const { return mTimer.getCurrentFPS(); }
        float getDeltaTime() const { return mTimer.getDeltaTime(); }
        double getElapsedTime() const { return mTimer.getElapsedTime(); }

        // Timer direct access
        Timer& getTimer() { return mTimer; }
        const Timer& getTimer() const { return mTimer; }

        // World reference access
        World& getWorld() { return *mWorld; }
        const World& getWorld() const { return *mWorld; }

        inline Window& getWindow() { return *mWindow; }
        inline static Application& get() { return *sInstance; }

    private:
        bool onWindowClose(WindowCloseEvent& e);
        bool onWindowResize(WindowResizeEvent& e);
        void createManagers();

        std::unique_ptr<Window> mWindow;
        ImGuiLayer* mImGuiLayer;
        World* mWorld; // Reference to World singleton
        Timer mTimer; // Timer instance
        bool mRunning = true;

    private:
        static Application* sInstance;
    };

    Application* createApplication();
}
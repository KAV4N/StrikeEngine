#pragma once

#include "Core.h"
#include "Window.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/ApplicationEvent.h"
#include "StrikeEngine/Events/KeyEvent.h"

namespace StrikeEngine {
    class World;

    class Application {
    public:
        friend class Renderer;

        Application();
        virtual ~Application();

        void run();
        void onEvent(Event& e);
        void onUpdate(float deltaTime);

        inline Window& getWindow() { return *mWindow; }
        inline static Application& get() { return *sInstance; }
        
        inline void setTargetFPS(int fps) { mTargetFPS = fps; }
        inline int getTargetFPS() const { return mTargetFPS; }
        inline float getCurrentFPS() const { return mCurrentFPS; }

    private:
        bool printProfiler(KeyPressedEvent& e);
        bool onWindowClose(WindowCloseEvent& e);
        bool onWindowResize(WindowResizeEvent& e);

        std::unique_ptr<Window> mWindow;
        bool mRunning = true;
        int mTargetFPS = 0;
        float mCurrentFPS = 0.0f;

        static Application* sInstance;
    };
}
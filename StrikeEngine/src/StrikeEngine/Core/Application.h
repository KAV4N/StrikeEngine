#pragma once

#include "Core.h"
#include "Window.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/ApplicationEvent.h"
#include "StrikeEngine/Events/KeyEvent.h"

namespace StrikeEngine {
    class World;

    /**
     * @brief The main application class that manages the application lifecycle.
     */
    class Application {
    public:
        Application();
        virtual ~Application();

        /**
         * @brief Run the main application loop.
         */
        void run();

        /**
         * @brief Get the application window.
         * @return Reference to the Window object.
         */
        inline Window& getWindow() { return *mWindow; }
        inline static Application& get() { return *sInstance; }
        
        
        inline void setTargetFPS(int fps) { mTargetFPS = fps; }
        inline int getTargetFPS() const { return mTargetFPS; }
        inline float getCurrentFPS() const { return mCurrentFPS; }

    private:
        friend class Renderer;

        bool printProfiler(KeyPressedEvent& e);
        bool onWindowClose(WindowCloseEvent& e);
        bool onWindowResize(WindowResizeEvent& e);

        void onEvent(Event& e);
        void onUpdate(float deltaTime);

    private:

        std::unique_ptr<Window> mWindow;
        bool mRunning = true;
        int mTargetFPS = 0;
        float mCurrentFPS = 0.0f;

        static Application* sInstance;
    };
}
#pragma once

#include "Core.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/ApplicationEvent.h"
#include "StrikeEngine/Events/KeyEvent.h"

#include "StrikeEngine/Core/Window.h"

#include <memory>

namespace Strike {
    class World;

    /**
     * @brief The main application class that manages the application lifecycle.
     */
    class Application {
    public:
        /**
         * @brief Construct a new Application object.
         */
        Application();
        
        /**
         * @brief Destroy the Application object.
         */
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
        
        /**
         * @brief Get the singleton Application instance.
         * @return Reference to the Application instance.
         */
        inline static Application& get() { return *sInstance; }
        
        /**
         * @brief Set the target frames per second.
         * @param fps Target FPS (0 for unlimited).
         */
        inline void setTargetFPS(int fps) { mTargetFPS = fps; }
        
        /**
         * @brief Get the target frames per second.
         * @return Target FPS.
         */
        inline int getTargetFPS() const { return mTargetFPS; }
        
        /**
         * @brief Get the current frames per second.
         * @return Current FPS.
         */
        inline float getCurrentFPS() const { return mCurrentFPS; }

        /**
         * @brief Get the Master Volume of the game
         * 
         * @return float volume
         */
        float getMasterVolume() const;

        /**
         * @brief Sets the global master volume.
         * 
         * @param volume Volume in range [0.0, 1.0].
         */
        void setMasterVolume(float volume);


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
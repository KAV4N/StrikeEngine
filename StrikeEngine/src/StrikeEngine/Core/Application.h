#pragma once

#include "Core.h"
#include "StrikeEngine/Events/Event.h"
#include "StrikeEngine/Events/ApplicationEvent.h"
#include "StrikeEngine/Events/KeyEvent.h"

#include "StrikeEngine/Core/Window.h"

#include <memory>

namespace Strike {
    class World;
    class Entity;

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
        inline Window& getWindow() { 
            STRIKE_ASSERT(sInstance, "Application instance not initialized");
            return *mWindow; 
        }
        
        /**
         * @brief Get the singleton Application instance.
         * @return Reference to the Application instance.
         */
        inline static Application& get() { 
            STRIKE_ASSERT(sInstance, "Application instance not initialized");
            return *sInstance; 
        }
        
        /**
         * @brief Set the target frames per second.
         * @param fps Target FPS (0 for unlimited).
         */
        inline void setTargetFPS(uint32_t fps) {
            STRIKE_ASSERT(sInstance, "Application instance not initialized");
            mTargetFPS = fps; 
        }
        
        /**
         * @brief Get the target frames per second.
         * @return Target FPS.
         */
        inline int getTargetFPS() const { 
            STRIKE_ASSERT(sInstance, "Application instance not initialized");
            return mTargetFPS; 
        }
        
        /**
         * @brief Get the current frames per second.
         * @return Current FPS.
         */
        inline float getCurrentFPS() const { 
            STRIKE_ASSERT(sInstance, "Application instance not initialized");
            return mCurrentFPS; 
        }

        /**
         * @brief Get the Master Volume of the game.
         * @return float volume in range [0.0, 1.0+].
         */
        float getMasterVolume() const;

        /**
         * @brief Sets the global master volume.
         * @param volume Volume in range [0.0, 1.0+].
         */
        void setMasterVolume(float volume);

        /**
         * @brief Returns the current RMS amplitude [0.0, 1.0] of the first
         *        playing sound on the given entity.
         *
         * Reads a small PCM window from miniaudio's data source without
         * affecting the actual playback cursor. Returns 0.0 if the entity
         * has no active AudioSource, no sound is currently playing, or
         * the AudioSystem is unavailable.
         *
         * @param entity          The entity that owns the AudioSourceComponent.
         * @param framesToSample  Number of PCM frames used for RMS calculation.
         */
        float getAudioAmplitude(const Entity& entity) const;

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
        uint32_t mTargetFPS = 0;
        float mCurrentFPS = 0.0f;

        static Application* sInstance;
    };
}
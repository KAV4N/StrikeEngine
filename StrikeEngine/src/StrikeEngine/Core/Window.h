#pragma once

#include "StrikeEngine/Core/Core.h"
#include "StrikeEngine/Events/Event.h"
#include <functional>
#include <string>
#include <memory>

// Forward declarations
struct GLFWwindow;

namespace StrikeEngine {

    // Forward declaration
    class GraphicsContext;

    /**
     * @brief Properties for creating a window.
     */
    struct WindowProps {
        std::string title;
        unsigned int width;
        unsigned int height;

        WindowProps(const std::string& title = "StrikeEngine",
            unsigned int width = 1280,
            unsigned int height = 720)
            : title(title), width(width), height(height) {
        }
    };

    /**
     * @brief Window class for StrikeEngine.
     */
    class Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        /**
         * @brief Construct a new Window object.
         * @param props Window properties (title, width, height).
         */
        Window(const WindowProps& props = WindowProps());
        
        /**
         * @brief Destroy the Window object.
         */
        ~Window();   
        
        /**
         * @brief Get the window width.
         * @return Current window width in pixels.
         */
        unsigned int getWidth() const;
        
        /**
         * @brief Get the window height.
         * @return Current window height in pixels.
         */
        unsigned int getHeight() const;        
        
        /**
         * @brief Enable or disable VSync.
         * @param enabled True to enable VSync, false to disable.
         */
        void setVSync(bool enabled);
        
        /**
         * @brief Set the window title.
         * @param title New window title.
         */
        void setWindowTitle(const std::string& title);
        
        /**
         * @brief Check if VSync is enabled.
         * @return True if VSync is enabled, false otherwise.
         */
        bool isVSync() const;
        
        /**
         * @brief Get the current time from GLFW.
         * @return Time in seconds since GLFW initialization.
         */
        double getTime() const;
        


    private:
        friend class Application;
        friend class Input;

        void init(const WindowProps& props);
        void shutdown();
        void checkResizeTimeout();

        void* getNativeWindow() const;
        void setEventCallback(const EventCallbackFn& callback);

        void onUpdate();
    private:
        struct WindowImpl;
        std::unique_ptr<WindowImpl> mImpl;
    };
}
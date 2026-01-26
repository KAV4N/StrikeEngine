#pragma once

#include "StrikeEngine/Core/Core.h"
#include "StrikeEngine/Events/Event.h"

namespace StrikeEngine {

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
     * @brief Abstract base class for a window in StrikeEngine.
     */
    class Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>;
        virtual ~Window() {}

        virtual void onUpdate() = 0;
        virtual unsigned int getWidth() const = 0;
        virtual unsigned int getHeight() const = 0;
        virtual void setEventCallback(const EventCallbackFn& callback) = 0;
        virtual void setVSync(bool enabled) = 0;
        virtual void setWindowTitle(std::string title) = 0;
        virtual bool isVSync() const = 0;
        virtual double getTime() const = 0;
        virtual void* getNativeWindow() const = 0;

        static Window* create(const WindowProps& props = WindowProps());
    };
}
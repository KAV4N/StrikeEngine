#pragma once

#include "StrikeEngine/Core/Window.h"
#include "StrikeEngine/Graphics/GraphicsContext.h"
#include <GLFW/glfw3.h>
#include <chrono>

namespace StrikeEngine {

    class WindowsWindow : public Window {
    public:
        WindowsWindow(const WindowProps& props);
        virtual ~WindowsWindow();

        void onUpdate() override;
        unsigned int getWidth() const override;
        unsigned int getHeight() const override;

        void setEventCallback(const EventCallbackFn& callback) override { mData.eventCallback = callback; }
        void setVSync(bool enabled) override;
        bool isVSync() const override;
        double getTime() const override;
        void setWindowTitle(std::string title) override;

        void* getNativeWindow() const override;

    private:
        void init(const WindowProps& props);
        void shutdown();
        void checkResizeTimeout();

        GLFWwindow* mWindow;
        std::unique_ptr<GraphicsContext> mGraphicsContext;

        struct WindowData {
            std::string title;
            unsigned int width;
            unsigned int height;
            bool vSync;
            EventCallbackFn eventCallback;
            
            // Resize throttling
            bool isResizing;
            unsigned int pendingWidth;
            unsigned int pendingHeight;
            std::chrono::steady_clock::time_point lastResizeTime;
        };
        WindowData mData;
        
        static constexpr int RESIZE_DELAY_MS = 150; // Delay in milliseconds
    };
}
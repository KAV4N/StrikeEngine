#include "strikepch.h"
#include "Window.h"
#include "StrikeEngine/Graphics/GraphicsContext.h"
#include "StrikeEngine/Events/KeyEvent.h"
#include "StrikeEngine/Events/MouseEvent.h"
#include "StrikeEngine/Events/ApplicationEvent.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <chrono>

namespace Strike {

    static bool sGLFWInitialized = false;
    static constexpr int RESIZE_DELAY_MS = 150;

    static void glfwErrorCallback(int error, const char* description) {
        STRIKE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    struct Window::WindowImpl {
        GLFWwindow* window;
        std::unique_ptr<GraphicsContext> graphicsContext;

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
        WindowData data;
    };

    Window::Window(const WindowProps& props) 
        : mImpl(std::make_unique<WindowImpl>()) {
        init(props);
    }

    Window::~Window() {
        shutdown();
    }

    void Window::setSize(unsigned int width, unsigned int height) {
        glfwSetWindowSize(mImpl->window, (int)width, (int)height);
        mImpl->data.width = width;
        mImpl->data.height = height;
    }

    void Window::init(const WindowProps& props) {
        mImpl->data.title = props.title;
        mImpl->data.width = props.width;
        mImpl->data.height = props.height;
        mImpl->data.isResizing = false;
        mImpl->data.pendingWidth = props.width;
        mImpl->data.pendingHeight = props.height;

        STRIKE_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

        if (!sGLFWInitialized) {
            int success = glfwInit();
            STRIKE_CORE_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(glfwErrorCallback);
            sGLFWInitialized = true;
        }

        mImpl->window = glfwCreateWindow((int)props.width, (int)props.height, mImpl->data.title.c_str(), nullptr, nullptr);

        mImpl->graphicsContext = GraphicsContext::create(mImpl->window);
        mImpl->graphicsContext->init();

        glfwSetWindowUserPointer(mImpl->window, &mImpl->data);
        setVSync(false);

        glfwSetWindowSizeCallback(mImpl->window, [](GLFWwindow* window, int width, int height) {
            WindowImpl::WindowData& data = *(WindowImpl::WindowData*)glfwGetWindowUserPointer(window);
            
            data.pendingWidth = width;
            data.pendingHeight = height;
            data.isResizing = true;
            data.lastResizeTime = std::chrono::steady_clock::now();
            
            data.width = width;
            data.height = height;
        });

        glfwSetWindowCloseCallback(mImpl->window, [](GLFWwindow* window) {
            WindowImpl::WindowData& data = *(WindowImpl::WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data.eventCallback(event);
        });

        glfwSetKeyCallback(mImpl->window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowImpl::WindowData& data = *(WindowImpl::WindowData*)glfwGetWindowUserPointer(window);

            switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event(key, 0);
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event(key);
                data.eventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event(key, 1);
                data.eventCallback(event);
                break;
            }
            }
        });

        glfwSetMouseButtonCallback(mImpl->window, [](GLFWwindow* window, int button, int action, int mods) {
            WindowImpl::WindowData& data = *(WindowImpl::WindowData*)glfwGetWindowUserPointer(window);

            switch (action) {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event(button);
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(button);
                data.eventCallback(event);
                break;
            }
            }
        });

        glfwSetScrollCallback(mImpl->window, [](GLFWwindow* window, double xOffset, double yOffset) {
            WindowImpl::WindowData& data = *(WindowImpl::WindowData*)glfwGetWindowUserPointer(window);
            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.eventCallback(event);
        });

        glfwSetCursorPosCallback(mImpl->window, [](GLFWwindow* window, double xPos, double yPos) {
            WindowImpl::WindowData& data = *(WindowImpl::WindowData*)glfwGetWindowUserPointer(window);
            MouseMovedEvent event((float)xPos, (float)yPos);
            data.eventCallback(event);
        });

        glfwSetCharCallback(mImpl->window, [](GLFWwindow* window, unsigned int keycode) {
            WindowImpl::WindowData& data = *(WindowImpl::WindowData*)glfwGetWindowUserPointer(window);
            KeyTypedEvent event(keycode);
            data.eventCallback(event);
        });
    }

    void Window::shutdown() {
        if (mImpl && mImpl->window) {
            glfwDestroyWindow(mImpl->window);
        }
    }

    void Window::onUpdate() {
        glfwPollEvents();
        checkResizeTimeout();
        glfwSwapBuffers(mImpl->window);
    }

     void Window::setEventCallback(const EventCallbackFn& callback) { mImpl->data.eventCallback = callback; }

    void Window::checkResizeTimeout() {
        if (mImpl->data.isResizing) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - mImpl->data.lastResizeTime).count();
            
            if (elapsed >= RESIZE_DELAY_MS) {
                mImpl->data.isResizing = false;
                
                WindowResizeEvent event(mImpl->data.pendingWidth, mImpl->data.pendingHeight);
                mImpl->data.eventCallback(event);
                
                STRIKE_CORE_INFO("Framebuffer resized to: {0}x{1}", mImpl->data.pendingWidth, mImpl->data.pendingHeight);
            }
        }
    }

    void Window::setVSync(bool enabled) {
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        mImpl->data.vSync = enabled;
    }

    bool Window::isVSync() const {
        return mImpl->data.vSync;
    }

    void Window::setWindowTitle(const std::string& title) {
        glfwSetWindowTitle(mImpl->window, title.c_str());
        mImpl->data.title = title;
    }

    unsigned int Window::getWidth() const {
        return mImpl->data.width;
    }

    unsigned int Window::getHeight() const {
        return mImpl->data.height;
    }

    double Window::getTime() const {
        return glfwGetTime();
    }

    void* Window::getNativeWindow() const {
        return mImpl->window;
    }
}
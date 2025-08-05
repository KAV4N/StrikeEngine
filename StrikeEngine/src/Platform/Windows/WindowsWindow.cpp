#include "strikepch.h"
#include "WindowsWindow.h"
#include "StrikeEngine/Events/KeyEvent.h"
#include "StrikeEngine/Events/MouseEvent.h"
#include "StrikeEngine/Events/ApplicationEvent.h"
#include <glad/glad.h>

namespace StrikeEngine {

    static bool sGLFWInitialized = false;

    WindowsWindow::WindowsWindow(const WindowProps& props) {
        init(props);
    }

    WindowsWindow::~WindowsWindow() {
        shutdown();
    }

    static void glfwErrorCallback(int error, const char* description) {
        STRIKE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    Window* Window::create(const WindowProps& props) {
        return new WindowsWindow(props);
    }

    void WindowsWindow::init(const WindowProps& props) {
        mData.title = props.title;
        mData.width = props.width;
        mData.height = props.height;

        STRIKE_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

        if (!sGLFWInitialized) {
            int success = glfwInit();
            STRIKE_CORE_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(glfwErrorCallback);
            sGLFWInitialized = true;
        }

        mWindow = glfwCreateWindow((int)props.width, (int)props.height, mData.title.c_str(), nullptr, nullptr);

        mGraphicsContext = GraphicsContext::create(mWindow);
        mGraphicsContext->init();

        glfwSetWindowUserPointer(mWindow, &mData);
        setVSync(false);

        glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int width, int height) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.width = width;
            data.height = height;

            WindowResizeEvent event(width, height);
            data.eventCallback(event);
            });

        glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data.eventCallback(event);
            });

        glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

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

        glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

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

        glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double xOffset, double yOffset) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.eventCallback(event);
            });

        glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xPos, double yPos) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseMovedEvent event((float)xPos, (float)yPos);
            data.eventCallback(event);
            });

        glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int keycode) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            KeyTypedEvent event(keycode);
            data.eventCallback(event);
            });
    }

    void WindowsWindow::shutdown() {
        glfwDestroyWindow(mWindow);
    }

    void WindowsWindow::onUpdate() {
        glfwPollEvents();
        glfwSwapBuffers(mWindow);
    }

    void WindowsWindow::setVSync(bool enabled) {
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        mData.vSync = enabled;
    }

    bool WindowsWindow::isVSync() const {
        return mData.vSync;
    }

    void WindowsWindow::setWindowTitle(std::string title) {
        glfwSetWindowTitle(mWindow, title.c_str());
        mData.title = title;
    }

    unsigned int WindowsWindow::getWidth() const {
        return mData.width;
    }

    unsigned int WindowsWindow::getHeight() const {
        return mData.height;
    }

    double WindowsWindow::getTime() const {
        return glfwGetTime();
    }

    void* WindowsWindow::getNativeWindow() const {
        return mWindow;
    }
}
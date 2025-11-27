#include "strikepch.h"
#include "GraphicsContext.h"
#include <glad/glad.h>

namespace StrikeEngine {

    GraphicsContext::GraphicsContext(GLFWwindow* windowHandle)
        : mWindowHandle(windowHandle) {
        STRIKE_CORE_ASSERT(windowHandle, "Window handle is null!");
    }

    void GraphicsContext::init() {
        glfwMakeContextCurrent(mWindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        STRIKE_CORE_ASSERT(status, "Failed to initialize Glad!");

        STRIKE_CORE_INFO("OpenGL Info:");
        STRIKE_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        STRIKE_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        STRIKE_CORE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        STRIKE_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "StrikeEngine requires at least OpenGL version 4.5!");
    }

    void GraphicsContext::swapBuffers() {
        glfwSwapBuffers(mWindowHandle);
    }

    std::unique_ptr<GraphicsContext> GraphicsContext::create(GLFWwindow* window) {
        return std::make_unique<GraphicsContext>(window);
    }
}
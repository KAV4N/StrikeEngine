#include "strikepch.h"
#include "OpenGLContext.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace StrikeEngine {

    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : mWindowHandle(windowHandle) {
        STRIKE_CORE_ASSERT(windowHandle, "Window handle is null!");
    }

    void OpenGLContext::init() {
        glfwMakeContextCurrent(mWindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        STRIKE_CORE_ASSERT(status, "Failed to initialize Glad!");

        STRIKE_CORE_INFO("OpenGL Info:");
        STRIKE_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        STRIKE_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        STRIKE_CORE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        STRIKE_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Hazel requires at least OpenGL version 4.5!");
    }

    void OpenGLContext::swapBuffers() {
        glfwSwapBuffers(mWindowHandle);
    }
}
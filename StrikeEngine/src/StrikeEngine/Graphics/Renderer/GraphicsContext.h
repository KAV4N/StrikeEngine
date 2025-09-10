#pragma once

#include <GLFW/glfw3.h>

struct GLFWwindow;

namespace StrikeEngine {

    class GraphicsContext {
    public:
        GraphicsContext(GLFWwindow* windowHandle);
        ~GraphicsContext() = default;

        void init();
        void swapBuffers();

        static std::unique_ptr<GraphicsContext> create(GLFWwindow* window);

    private:
        GLFWwindow* mWindowHandle;
    };
}
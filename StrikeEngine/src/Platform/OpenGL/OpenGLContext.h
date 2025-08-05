#pragma once

#include "StrikeEngine/Graphics/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace StrikeEngine {

    class OpenGLContext : public GraphicsContext {
    public:
        OpenGLContext(GLFWwindow* windowHandle);

        void init() override;
        void swapBuffers() override;

    private:
        GLFWwindow* mWindowHandle;
    };
}
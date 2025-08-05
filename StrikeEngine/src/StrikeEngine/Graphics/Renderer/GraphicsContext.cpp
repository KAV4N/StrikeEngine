#include "strikepch.h"
#include "GraphicsContext.h"
#include "StrikeEngine/Graphics/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace StrikeEngine {

    std::unique_ptr<GraphicsContext> GraphicsContext::create(void* window) {
        switch (Renderer::getApi()) {
        case RendererAPI::Api::None:
            STRIKE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::Api::OpenGL:
            return std::make_unique<OpenGLContext>(static_cast<GLFWwindow*>(window));
        }

        STRIKE_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
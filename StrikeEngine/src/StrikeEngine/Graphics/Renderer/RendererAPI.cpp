#include "strikepch.h"
#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace StrikeEngine {

    RendererAPI::Api RendererAPI::sApi = RendererAPI::Api::OpenGL;

    std::unique_ptr<RendererAPI> RendererAPI::create() {
        switch (sApi) {
        case RendererAPI::Api::None:
            STRIKE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::Api::OpenGL:
            return std::make_unique<OpenGLRendererAPI>();
        }

        STRIKE_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
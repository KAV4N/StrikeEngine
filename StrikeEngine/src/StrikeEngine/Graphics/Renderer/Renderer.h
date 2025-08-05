#pragma once

#include "StrikeEngine/Graphics/Renderer/RendererAPI.h"

namespace StrikeEngine {

    class Renderer {
    public:
        static RendererAPI::Api getApi() { return RendererAPI::getApi(); }
    };
}
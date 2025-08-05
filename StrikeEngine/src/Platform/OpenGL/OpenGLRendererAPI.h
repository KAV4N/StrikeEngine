#pragma once

#include "StrikeEngine/Graphics/Renderer/RendererAPI.h"

namespace StrikeEngine {

    class OpenGLRendererAPI : public RendererAPI {
    public:
        void init() override;
        void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
        void setClearColor(const glm::vec4& color);
        void clear();
    };
}
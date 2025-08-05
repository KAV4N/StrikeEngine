#pragma once

#include "StrikeEngine/Graphics/Texture2D.h"

namespace StrikeEngine {

    class OpenGLTexture2D : public Texture2D {
    public:
        OpenGLTexture2D(const Properties& properties);
        ~OpenGLTexture2D() override;

        void setData(void* data, uint32_t size) override;

    private:
        uint32_t mRendererId = 0;
        Properties mProperties;
    };
}
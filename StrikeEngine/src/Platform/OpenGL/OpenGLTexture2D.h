#pragma once

#include "StrikeEngine/Graphics/Renderer/Texture2D.h"

namespace StrikeEngine {

    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(const Properties& properties);
        virtual ~OpenGLTexture2D();

        void SetData(void* data, uint32_t size) override;

    private:
        uint32_t m_RendererID = 0;
        Properties m_Properties;
    };

}

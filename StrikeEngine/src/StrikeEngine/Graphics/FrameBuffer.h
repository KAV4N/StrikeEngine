#pragma once
#include <cstdint>
#include <glad/glad.h>

namespace StrikeEngine {
    class FrameBuffer
    {
    public:
        FrameBuffer(uint32_t width, uint32_t height);
        ~FrameBuffer();
        GLuint getColorTextureID() const { return mColorTexture; }
        GLuint getDepthTextureID() const { return mDepthTexture; }
        uint32_t getWidth() const { return mWidth; }
        uint32_t getHeight() const { return mHeight; }
        void bind();
        void unBind();
        void resize(uint32_t width, uint32_t height);
        void bindFramebufferTexture(int slot);
        void bindFramebufferDepthTexture(int slot);
    private:
        GLuint mRendererID = 0;
        GLuint mColorTexture = 0;
        GLuint mDepthTexture = 0;
        uint32_t mWidth;
        uint32_t mHeight;
    private:
        void invalidate();
        static const uint32_t sMaxFramebufferSize = 8192;
    };
}
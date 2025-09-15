#pragma once
#include <cstdint>
#include "glad/glad.h"

namespace StrikeEngine {
    class FrameBuffer
    {
    public:
        FrameBuffer(uint32_t width, uint32_t height);
        ~FrameBuffer();
        GLuint getColorTextureID() { return mColorTexture; }
        GLuint getDepthTextureID() { return mDepthTexture; }
        uint32_t getWidth() { return mWidth; }
        uint32_t getHeight() { return mHeight; }
        void bind();
        void unBind();
        void resize(uint32_t width, uint32_t height);
        void clearFrameBuffer();
        void bindFramebufferTexture(int slot);
        void bindFramebufferDepthTexture(int slot);
    private:
        GLuint mRenderID = 0;
        GLuint mColorTexture = 0;
        GLuint mDepthTexture = 0;
        uint32_t mWidth;
        uint32_t mHeight;
    private:
        void destroy();
        void create();
    };
}
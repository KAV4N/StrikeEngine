#include "strikepch.h"
#include "FrameBuffer.h"

namespace StrikeEngine {
    FrameBuffer::FrameBuffer(uint32_t width, uint32_t height)
        : mWidth(width), mHeight(height)
    {
        invalidate();
    }

    FrameBuffer::~FrameBuffer()
    {
        glDeleteFramebuffers(1, &mRendererID);
        glDeleteTextures(1, &mColorTexture);
        glDeleteTextures(1, &mDepthTexture);
    }


    void FrameBuffer::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void FrameBuffer::bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);
    }

    void FrameBuffer::unBind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0 || width > sMaxFramebufferSize || height > sMaxFramebufferSize) {
            STRIKE_CORE_ERROR("Invalid framebuffer size: {0}x{1}", width, height);
            return;
        }
        mWidth = width;
        mHeight = height;
        invalidate();
    }

  
    void FrameBuffer::bindFramebufferTexture(int slot)
    {
        glBindTextureUnit(slot, mColorTexture);
    }

    void FrameBuffer::bindFramebufferDepthTexture(int slot)
    {
        glBindTextureUnit(slot, mDepthTexture);
    }

    void FrameBuffer::invalidate()
    {
        if (mRendererID)
        {
            glDeleteFramebuffers(1, &mRendererID);
            glDeleteTextures(1, &mColorTexture);
            glDeleteTextures(1, &mDepthTexture);
        }
        
        glCreateFramebuffers(1, &mRendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);

        glCreateTextures(GL_TEXTURE_2D, 1, &mColorTexture);
        glBindTexture(GL_TEXTURE_2D, mColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture, 0);

        glCreateTextures(GL_TEXTURE_2D, 1, &mDepthTexture);
        glBindTexture(GL_TEXTURE_2D, mDepthTexture);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, mWidth, mHeight);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
    }
}
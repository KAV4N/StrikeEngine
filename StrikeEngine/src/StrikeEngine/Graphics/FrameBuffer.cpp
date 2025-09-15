#include "strikepch.h"
#include "FrameBuffer.h"

#include "StrikeEngine/Core/Log.h"

namespace StrikeEngine {
    FrameBuffer::FrameBuffer(uint32_t width, uint32_t height)
        : mWidth(width), mHeight(height)
    {
        create();
    }

    FrameBuffer::~FrameBuffer()
    {
        destroy();
    }

    void FrameBuffer::bind()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mRenderID);
        STRIKE_CORE_ERROR(glGetError());
    }

    void FrameBuffer::unBind()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    void FrameBuffer::resize(uint32_t width, uint32_t height)
    {
        mWidth = width;
        mHeight = height;
        destroy();
        create();
        glViewport(0, 0, width, height);
    }

    void FrameBuffer::clearFrameBuffer()
    {
        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glClearBufferfv(GL_COLOR, 0, clearColor);
        glFlush();
    }

    void FrameBuffer::bindFramebufferTexture(int slot)
    {
        glBindTextureUnit(slot, mColorTexture);
    }

    void FrameBuffer::bindFramebufferDepthTexture(int slot)
    {
        glBindTextureUnit(slot, mDepthTexture);
    }

    void FrameBuffer::destroy()
    {
        if (mRenderID)
        {
            glDeleteFramebuffers(1, &mRenderID);
            glDeleteTextures(1, &mColorTexture);
            glDeleteTextures(1, &mDepthTexture);
        }
    }

    void FrameBuffer::create()
    {
        glGenFramebuffers(1, &mRenderID);
        glBindFramebuffer(GL_FRAMEBUFFER, mRenderID);

        glGenTextures(1, &mColorTexture);
        glBindTexture(GL_TEXTURE_2D, mColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture, 0);
        GLenum buffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, buffers);

        glGenTextures(1, &mDepthTexture);
        glBindTexture(GL_TEXTURE_2D, mDepthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
            STRIKE_CORE_INFO("Color FrameBuffer Complete");
        else
            STRIKE_CORE_ERROR("Color FrameBuffer Incomplete !!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
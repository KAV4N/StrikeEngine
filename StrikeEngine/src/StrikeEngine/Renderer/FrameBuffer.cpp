#include "strikepch.h"
#include "FrameBuffer.h"
#include <iostream>

namespace StrikeEngine {

    FrameBuffer::FrameBuffer(int width, int height)
        : m_Width(width), m_Height(height), m_HasDepthAttachment(false),
        m_HasStencilAttachment(false), m_HasShadowMap(false) {
        Create();
    }

    FrameBuffer::~FrameBuffer() {
        Destroy();
    }

    void FrameBuffer::Create() {
        glGenFramebuffers(1, &m_FBO);
    }


    void FrameBuffer::Destroy() {
        glDeleteFramebuffers(1, &m_FBO);
        if (m_HasDepthAttachment) {
            glDeleteTextures(1, &m_DepthAttachment);
        }
        if (m_HasStencilAttachment) {
            glDeleteTextures(1, &m_StencilAttachment);
        }
        if (m_HasShadowMap) {
            glDeleteTextures(1, &m_ShadowMap);
        }

        for (auto colorAttachment : m_ColorAttachments) {
            glDeleteTextures(1, &colorAttachment);
        }
    }

    void FrameBuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    }

    void FrameBuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Resize(int width, int height) {
        m_Width = width;
        m_Height = height;
        Destroy();
        Create();
    }

    void FrameBuffer::AddColorAttachment(GLenum internalFormat, GLenum format, GLenum type) {
        unsigned int colorAttachment;
        glGenTextures(1, &colorAttachment);
        glBindTexture(GL_TEXTURE_2D, colorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, format, type, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_ColorAttachments.size(), GL_TEXTURE_2D, colorAttachment, 0);
        m_ColorAttachments.push_back(colorAttachment);
        CheckFramebufferStatus();
    }

    void FrameBuffer::SetDepthAttachment(GLenum internalFormat) {
        glGenTextures(1, &m_DepthAttachment);
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
        m_HasDepthAttachment = true;
        CheckFramebufferStatus();
    }

    void FrameBuffer::SetStencilAttachment(GLenum internalFormat) {
        glGenTextures(1, &m_StencilAttachment);
        glBindTexture(GL_TEXTURE_2D, m_StencilAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, GL_STENCIL_ATTACHMENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_StencilAttachment, 0);
        m_HasStencilAttachment = true;
        CheckFramebufferStatus();
    }

    void FrameBuffer::SetShadowMap(GLenum internalFormat) {
        if (m_HasShadowMap) {
            glDeleteTextures(1, &m_ShadowMap);
        }

        glGenTextures(1, &m_ShadowMap);
        glBindTexture(GL_TEXTURE_2D, m_ShadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        Unbind();

        m_HasShadowMap = true;
        CheckFramebufferStatus();
    }



    unsigned int FrameBuffer::GetColorAttachment(int index) const {
        return m_ColorAttachments[index];
    }

    unsigned int FrameBuffer::GetDepthAttachment() const {
        return m_DepthAttachment;
    }

    unsigned int FrameBuffer::GetStencilAttachment() const {
        return m_StencilAttachment;
    }

    unsigned int FrameBuffer::GetShadowMap() const {
        return m_ShadowMap;
    }

    void FrameBuffer::CheckFramebufferStatus() {
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer incomplete: ";
            switch (status) {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: std::cerr << "Incomplete attachment."; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: std::cerr << "Missing attachment."; break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: std::cerr << "Incomplete draw buffer."; break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: std::cerr << "Incomplete read buffer."; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: std::cerr << "Unsupported framebuffer format."; break;
            default: std::cerr << "Unknown error."; break;
            }
            std::cerr << std::endl;
        }
    }

}

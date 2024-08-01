#pragma once

#include <glad/glad.h>
#include <vector>

namespace StrikeEngine {

    class FrameBuffer {
    public:
        FrameBuffer(int width, int height, bool useDepthTexture = true);
        ~FrameBuffer();

        void Create();
        void Destroy();
        void Bind();
        void Unbind();
        void Resize(int width, int height);
        void AddColorAttachment(GLenum internalFormat, GLenum format, GLenum type);
        void SetDepthAttachment(GLenum internalFormat);
        void SetStencilAttachment(GLenum internalFormat);
        void SetShadowMap(GLenum internalFormat);

        unsigned int GetColorAttachment(int index) const;
        unsigned int GetDepthAttachment() const;
        unsigned int GetStencilAttachment() const;
        unsigned int GetShadowMap() const;

    private:
        void CheckFramebufferStatus();

        unsigned int m_FBO;
        unsigned int m_DepthAttachment;
        unsigned int m_StencilAttachment;
        unsigned int m_ShadowMap;

        std::vector<unsigned int> m_ColorAttachments;
        int m_Width, m_Height;
        bool m_HasDepthAttachment, m_HasStencilAttachment, m_HasShadowMap;
    };

}

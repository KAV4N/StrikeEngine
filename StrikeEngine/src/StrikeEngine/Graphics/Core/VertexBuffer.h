#pragma once
#include <glad/glad.h>

namespace StrikeEngine {

    class VertexBuffer {
    public:
        VertexBuffer(const void* data, unsigned int size, GLenum usage = GL_STATIC_DRAW);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;
        void UpdateData(const void* data, unsigned int size);

    private:
        GLuint m_RendererID;
    };

}
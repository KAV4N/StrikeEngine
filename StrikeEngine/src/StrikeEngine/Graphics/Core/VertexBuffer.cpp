#include "VertexBuffer.h"

namespace StrikeEngine {

    VertexBuffer::VertexBuffer(const void* data, unsigned int size, GLenum usage) {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    }

    VertexBuffer::~VertexBuffer() {
        glDeleteBuffers(1, &m_RendererID);
    }

    void VertexBuffer::Bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    void VertexBuffer::Unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::UpdateData(const void* data, unsigned int size) {
        Bind();
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }

}
#include "UniformBuffer.h"

namespace StrikeEngine {

    UniformBuffer::UniformBuffer(unsigned int size, GLenum usage) {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, usage);
    }

    UniformBuffer::~UniformBuffer() {
        glDeleteBuffers(1, &m_RendererID);
    }

    void UniformBuffer::Bind() const {
        glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    }

    void UniformBuffer::Unbind() const {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UniformBuffer::SetData(const void* data, unsigned int size, unsigned int offset) {
        Bind();
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    }

    void UniformBuffer::BindBase(unsigned int bindingPoint) {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_RendererID);
    }

}
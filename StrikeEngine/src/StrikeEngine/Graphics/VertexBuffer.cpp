#include "VertexBuffer.h"

namespace StrikeEngine {

    VertexBuffer::VertexBuffer() : mHandle(0) {
        glGenBuffers(1, &mHandle);
    }

    VertexBuffer::~VertexBuffer() {
        cleanup();
    }

    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept : mHandle(other.mHandle) {
        other.mHandle = 0;
    }

    VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept {
        if (this != &other) {
            cleanup();
            mHandle = other.mHandle;
            other.mHandle = 0;
        }
        return *this;
    }

    void VertexBuffer::bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, mHandle);
    }

    void VertexBuffer::unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::setData(const void* data, size_t size, GLenum usage) {
        bind();
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    }

    void VertexBuffer::cleanup() {
        if (mHandle != 0) {
            glDeleteBuffers(1, &mHandle);
            mHandle = 0;
        }
    }

} 
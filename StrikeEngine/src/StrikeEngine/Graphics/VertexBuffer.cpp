#include "VertexBuffer.h"

namespace StrikeEngine {

    VertexBuffer::VertexBuffer() : mRendererID(0) {
        glGenBuffers(1, &mRendererID);
    }

    VertexBuffer::~VertexBuffer() {
        cleanup();
    }

    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept : mRendererID(other.mRendererID) {
        other.mRendererID = 0;
    }

    VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept {
        if (this != &other) {
            cleanup();
            mRendererID = other.mRendererID;
            other.mRendererID = 0;
        }
        return *this;
    }

    void VertexBuffer::bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, mRendererID);
    }

    void VertexBuffer::unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::setData(const void* data, size_t size, GLenum usage) {
        bind();
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    }

    void VertexBuffer::cleanup() {
        if (mRendererID != 0) {
            glDeleteBuffers(1, &mRendererID);
            mRendererID = 0;
        }
    }

} 
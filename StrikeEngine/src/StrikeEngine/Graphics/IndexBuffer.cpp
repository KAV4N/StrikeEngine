#include "IndexBuffer.h"

namespace StrikeEngine {

    IndexBuffer::IndexBuffer() : mRendererID(0), mCount(0) {
        glGenBuffers(1, &mRendererID);
    }

    IndexBuffer::~IndexBuffer() {
        cleanup();
    }

    IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept : mRendererID(other.mRendererID), mCount(other.mCount) {
        other.mRendererID = 0;
        other.mCount = 0;
    }

    IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept {
        if (this != &other) {
            cleanup();
            mRendererID = other.mRendererID;
            mCount = other.mCount;
            other.mRendererID = 0;
            other.mCount = 0;
        }
        return *this;
    }

    void IndexBuffer::bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRendererID);
    }

    void IndexBuffer::unbind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void IndexBuffer::setData(const uint32_t* indices, size_t count, GLenum usage) {
        mCount = count;
        bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, usage);
    }

    void IndexBuffer::cleanup() {
        if (mRendererID != 0) {
            glDeleteBuffers(1, &mRendererID);
            mRendererID = 0;
            mCount = 0;
        }
    }

} // namespace StrikeEngine
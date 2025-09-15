#pragma once
#include <glad/glad.h>
#include <cstdint>

namespace StrikeEngine {

    class IndexBuffer {
    public:
        IndexBuffer();
        ~IndexBuffer();

        // Non-copyable
        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;

        // Movable
        IndexBuffer(IndexBuffer&& other) noexcept;
        IndexBuffer& operator=(IndexBuffer&& other) noexcept;

        void bind() const;
        void unbind() const;

        void setData(const uint32_t* indices, size_t count, GLenum usage = GL_STATIC_DRAW);

        GLuint getID() const { return mRendererID; }
        bool isValid() const { return mRendererID != 0; }
        size_t getCount() const { return mCount; }

    private:
        GLuint mRendererID;
        size_t mCount;
        void cleanup();
    };

} 
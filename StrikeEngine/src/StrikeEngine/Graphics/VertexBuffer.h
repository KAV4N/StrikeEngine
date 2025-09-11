#pragma once
#include <glad/glad.h>
#include <cstdint>

namespace StrikeEngine {

    class VertexBuffer {
    public:
        VertexBuffer();
        ~VertexBuffer();

        // Non-copyable
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        // Movable
        VertexBuffer(VertexBuffer&& other) noexcept;
        VertexBuffer& operator=(VertexBuffer&& other) noexcept;

        void bind() const;
        void unbind() const;

        void setData(const void* data, size_t size, GLenum usage = GL_STATIC_DRAW);

        GLuint getID() const { return mRendererID; }
        bool isValid() const { return mRendererID != 0; }

    private:
        GLuint mRendererID;
        void cleanup();
    };

} // namespace StrikeEngine
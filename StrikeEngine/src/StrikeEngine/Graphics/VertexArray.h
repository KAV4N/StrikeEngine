#pragma once
#include <glad/glad.h>

namespace StrikeEngine {

    class VertexBuffer;
    class IndexBuffer;
    struct Vertex;

    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        // Non-copyable
        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        // Movable
        VertexArray(VertexArray&& other) noexcept;
        VertexArray& operator=(VertexArray&& other) noexcept;

        void bind() const;
        void unbind() const;

        void setVertexBuffer(const VertexBuffer& vertexBuffer);
        void setIndexBuffer(const IndexBuffer& indexBuffer);

        GLuint getHandle() const { return mHandle; }
        bool isValid() const { return mHandle != 0; }

    private:
        GLuint mHandle;
        void cleanup();
        void setupVertexAttributes();
    };

} // namespace StrikeEngine
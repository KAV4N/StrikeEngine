#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "StrikeEngine/Asset/Types/Mesh.h"

namespace StrikeEngine {

    VertexArray::VertexArray() : mRendererID(0) {
        glGenVertexArrays(1, &mRendererID);
    }

    VertexArray::~VertexArray() {
        cleanup();
    }

    VertexArray::VertexArray(VertexArray&& other) noexcept : mRendererID(other.mRendererID) {
        other.mRendererID = 0;
    }

    VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
        if (this != &other) {
            cleanup();
            mRendererID = other.mRendererID;
            other.mRendererID = 0;
        }
        return *this;
    }

    void VertexArray::bind() const {
        glBindVertexArray(mRendererID);
    }

    void VertexArray::unbind() const {
        glBindVertexArray(0);
    }

    void VertexArray::setVertexBuffer(const VertexBuffer& vertexBuffer) {
        bind();
        vertexBuffer.bind();
        setupVertexAttributes();
    }

    void VertexArray::setIndexBuffer(const IndexBuffer& indexBuffer) {
        bind();
        indexBuffer.bind();
    }

    void VertexArray::setupVertexAttributes() {
        // Position (vec3)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
        // TexCoord (vec2)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoord));
        // Normal (vec3)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
        // Tangent (vec3)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tangent));
        // BiNormal (vec3)
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, biNormal));
    }

    void VertexArray::cleanup() {
        if (mRendererID != 0) {
            glDeleteVertexArrays(1, &mRendererID);
            mRendererID = 0;
        }
    }

} // namespace StrikeEngine
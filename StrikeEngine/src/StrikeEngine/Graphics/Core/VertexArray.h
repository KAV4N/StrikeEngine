#pragma once
#include <glad/glad.h>
#include <vector>
#include "VertexBuffer.h"

namespace StrikeEngine {
    struct VertexBufferElement {
        uint32_t type;
        uint32_t count;
        uint8_t normalized;

        static uint32_t GetSizeOfType(uint32_t type) {
            switch (type) {
            case GL_FLOAT: return sizeof(float);
            case GL_UNSIGNED_INT: return sizeof(uint32_t);
            case GL_UNSIGNED_BYTE: return sizeof(uint8_t);
            }
            return 0;
        }
    };

    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        void Bind() const;
        void Unbind() const;
        void AddBuffer(const VertexBuffer& vb, const std::vector<VertexBufferElement>& elements);

    private:
        uint32_t m_RendererID;
        uint32_t m_StrideSize;
    };
}
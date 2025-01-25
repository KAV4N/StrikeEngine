#pragma once
#include <memory>
#include <vector>
#include <glad/glad.h>
#include "VertexBuffer.h"

namespace StrikeEngine {

    class VertexArray {
    public:
        struct VertexBufferElement {
            unsigned int type;
            unsigned int count;
            unsigned char normalized;
        };

        VertexArray();
        ~VertexArray();

        void AddBuffer(const VertexBuffer& vb, const std::vector<VertexBufferElement>& elements);
        void Bind() const;
        void Unbind() const;

    private:
        GLuint m_RendererID;
        std::vector<VertexBufferElement> m_Elements;
    };

}
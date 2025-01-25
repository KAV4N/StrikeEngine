#include "VertexArray.h"

namespace StrikeEngine {

    VertexArray::VertexArray() {
        glGenVertexArrays(1, &m_RendererID);
    }

    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void VertexArray::AddBuffer(const VertexBuffer& vb, const std::vector<VertexBufferElement>& elements) {
        Bind();
        vb.Bind();

        m_Elements = elements;
        unsigned int offset = 0;
        for (unsigned int i = 0; i < elements.size(); i++) {
            const auto& element = elements[i];
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, element.count, element.type, element.normalized,
                0, (const void*)offset);
            offset += element.count * sizeof(element.type);
        }
    }

    void VertexArray::Bind() const {
        glBindVertexArray(m_RendererID);
    }

    void VertexArray::Unbind() const {
        glBindVertexArray(0);
    }

}
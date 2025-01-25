#pragma once
#include <glad/glad.h>

namespace StrikeEngine {

    class IndexBuffer {
    public:
        IndexBuffer(const unsigned int* data, unsigned int count);
        ~IndexBuffer();

        void Bind() const;
        void Unbind() const;
        unsigned int GetCount() const { return m_Count; }

    private:
        GLuint m_RendererID;
        unsigned int m_Count;
    };

}
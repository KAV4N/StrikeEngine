#pragma once
#include <glad/glad.h>

namespace StrikeEngine {
    class VertexBuffer {
    public:
        VertexBuffer(const void* data, uint32_t size);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;
        void SetData(const void* data, uint32_t size);

    private:
        uint32_t m_RendererID;
    };
}
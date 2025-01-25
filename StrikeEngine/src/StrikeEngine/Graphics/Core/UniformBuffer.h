#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace StrikeEngine {

    class UniformBuffer {
    public:
        UniformBuffer(unsigned int size, GLenum usage = GL_DYNAMIC_DRAW);
        ~UniformBuffer();

        void Bind() const;
        void Unbind() const;
        void SetData(const void* data, unsigned int size, unsigned int offset = 0);
        void BindBase(unsigned int bindingPoint);

    private:
        GLuint m_RendererID;
    };

}
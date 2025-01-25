#include "Texture.h"
#include <glad/glad.h>

namespace StrikeEngine {

    Texture::Texture(unsigned int id, TextureType type, TextureUnit unit,
        int width, int height, const std::string& path)
        : m_TextureID(id), m_Type(type), m_Unit(unit),
        m_Width(width), m_Height(height), m_Path(path) {
    }

    Texture::~Texture() {
        glDeleteTextures(1, &m_TextureID);
    }

    void Texture::Bind() const {
        glActiveTexture(m_Unit);
        glBindTexture(m_Type == TextureType::CUBE_MAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, m_TextureID);
    }

    void Texture::SetData(const void* data, TextureTarget target) const {
        Bind();
        GLenum glTarget = target == TextureTarget::AUTO ?
            (m_Type == TextureType::CUBE_MAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D) :
            static_cast<GLenum>(target);

        if (m_Type == TextureType::CUBE_MAP && target == TextureTarget::AUTO) {
            for (int i = 0; i < 6; ++i) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                    m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            }
        }
        else {
            glTexImage2D(glTarget, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
    }

    void Texture::SetParameter(TextureTarget target, TextureParameterName paramName, TextureParameter paramValue) const {
        Bind();
        glTexParameteri(target == TextureTarget::AUTO ?
            (m_Type == TextureType::CUBE_MAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D) :
            static_cast<GLenum>(target),
            static_cast<GLenum>(paramName),
            static_cast<GLenum>(paramValue));
    }

    void Texture::GenerateMipmap(TextureTarget target) const {
        Bind();
        glGenerateMipmap(target == TextureTarget::AUTO ?
            (m_Type == TextureType::CUBE_MAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D) :
            static_cast<GLenum>(target));
    }

}
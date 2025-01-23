#include "strikepch.h"
#include "Texture.h"
#include <stb_image.h>
#include <iostream>

namespace StrikeEngine {

    Texture::Texture(Type type)
        : m_TextureID(0), m_Type(type), m_Width(0), m_Height(0), m_Channels(0)
    {
        glGenTextures(1, &m_TextureID);
    }

    Texture::~Texture() {
        if (m_TextureID != 0) {
            glDeleteTextures(1, &m_TextureID);
        }
    }

    Texture::Texture(Texture&& other) noexcept
        : m_TextureID(other.m_TextureID), m_Type(other.m_Type),
        m_Width(other.m_Width), m_Height(other.m_Height), m_Channels(other.m_Channels)
    {
        other.m_TextureID = 0;
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        if (this != &other) {
            if (m_TextureID != 0) {
                glDeleteTextures(1, &m_TextureID);
            }

            m_TextureID = other.m_TextureID;
            m_Type = other.m_Type;
            m_Width = other.m_Width;
            m_Height = other.m_Height;
            m_Channels = other.m_Channels;

            other.m_TextureID = 0;
        }
        return *this;
    }

    bool Texture::LoadFromFile(const std::string& path) {
        Bind();

        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);

        if (!data) {
            std::cerr << "Failed to load texture: " << path << std::endl;
            return false;
        }

        Format format = (m_Channels == 4) ? Format::RGBA : Format::RGB;
        GLenum glFormat = ConvertFormatToGL(format, m_Channels);

        glTexImage2D(GL_TEXTURE_2D, 0, glFormat, m_Width, m_Height, 0, glFormat, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);

        return true;
    }

    bool Texture::LoadCubemap(const std::vector<std::string>& faces) {
        if (m_Type != Type::TextureCubeMap) {
            std::cerr << "Texture type must be CubeMap to load cubemap faces" << std::endl;
            return false;
        }

        Bind();

        stbi_set_flip_vertically_on_load(false);
        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char* data = stbi_load(faces[i].c_str(), &m_Width, &m_Height, &m_Channels, 0);
            if (!data) {
                std::cerr << "Failed to load cubemap face: " << faces[i] << std::endl;
                return false;
            }

            Format format = (m_Channels == 4) ? Format::RGBA : Format::RGB;
            GLenum glFormat = ConvertFormatToGL(format, m_Channels);

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glFormat,
                m_Width, m_Height, 0, glFormat, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }

        return true;
    }

    void Texture::SetFilterMode(FilterMode minFilter, FilterMode magFilter) {
        Bind();
        glTexParameteri(ConvertTypeToGL(m_Type), GL_TEXTURE_MIN_FILTER, ConvertFilterToGL(minFilter));
        glTexParameteri(ConvertTypeToGL(m_Type), GL_TEXTURE_MAG_FILTER, ConvertFilterToGL(magFilter));
    }

    void Texture::SetWrapMode(WrapMode sWrap, WrapMode tWrap, WrapMode rWrap) {
        Bind();
        glTexParameteri(ConvertTypeToGL(m_Type), GL_TEXTURE_WRAP_S, ConvertWrapToGL(sWrap));
        glTexParameteri(ConvertTypeToGL(m_Type), GL_TEXTURE_WRAP_T, ConvertWrapToGL(tWrap));
        if (m_Type == Type::Texture3D || m_Type == Type::TextureCubeMap) {
            glTexParameteri(ConvertTypeToGL(m_Type), GL_TEXTURE_WRAP_R, ConvertWrapToGL(rWrap));
        }
    }

    void Texture::SetBorderColor(float r, float g, float b, float a) {
        Bind();
        float borderColor[] = { r, g, b, a };
        glTexParameterfv(ConvertTypeToGL(m_Type), GL_TEXTURE_BORDER_COLOR, borderColor);
    }

    void Texture::GenerateMipmap() {
        Bind();
        glGenerateMipmap(ConvertTypeToGL(m_Type));
    }

    void Texture::Bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(ConvertTypeToGL(m_Type), m_TextureID);
    }

    void Texture::Unbind() const {
        glBindTexture(ConvertTypeToGL(m_Type), 0);
    }

    GLenum Texture::ConvertTypeToGL(Type type) {
        switch (type) {
        case Type::Texture1D: return GL_TEXTURE_1D;
        case Type::Texture2D: return GL_TEXTURE_2D;
        case Type::Texture3D: return GL_TEXTURE_3D;
        case Type::TextureCubeMap: return GL_TEXTURE_CUBE_MAP;
        case Type::Texture2DArray: return GL_TEXTURE_2D_ARRAY;
        case Type::TextureRectangle: return GL_TEXTURE_RECTANGLE;
        default: return GL_TEXTURE_2D;
        }
    }

    GLenum Texture::ConvertFilterToGL(FilterMode filter) {
        switch (filter) {
        case FilterMode::Nearest: return GL_NEAREST;
        case FilterMode::Linear: return GL_LINEAR;
        case FilterMode::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
        case FilterMode::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
        case FilterMode::NearestMipmapLinear: return GL_NEAREST_MIPMAP_LINEAR;
        case FilterMode::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
        default: return GL_LINEAR;
        }
    }

    GLenum Texture::ConvertWrapToGL(WrapMode wrap) {
        switch (wrap) {
        case WrapMode::Repeat: return GL_REPEAT;
        case WrapMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
        case WrapMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
        case WrapMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
        default: return GL_REPEAT;
        }
    }

    GLenum Texture::ConvertFormatToGL(Format format, int channels) {
        switch (format) {
        case Format::RGB: return GL_RGB;
        case Format::RGBA: return GL_RGBA;
        case Format::DEPTH: return GL_DEPTH_COMPONENT;
        case Format::DEPTH_STENCIL: return GL_DEPTH_STENCIL;
        default: return (channels == 4) ? GL_RGBA : GL_RGB;
        }
    }
}

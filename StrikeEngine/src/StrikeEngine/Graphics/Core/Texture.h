#pragma once
#include <string>
#include <glad/glad.h>
#include <vector>

namespace StrikeEngine {
    class Texture {
    public:
        enum class Type {
            Texture1D,
            Texture2D,
            Texture3D,
            TextureCubeMap,
            Texture2DArray,
            TextureRectangle
        };

        enum class FilterMode {
            Nearest,
            Linear,
            NearestMipmapNearest,
            LinearMipmapNearest,
            NearestMipmapLinear,
            LinearMipmapLinear
        };

        enum class WrapMode {
            Repeat,
            MirroredRepeat,
            ClampToEdge,
            ClampToBorder
        };

        enum class Format {
            RGB,
            RGBA,
            DEPTH,
            DEPTH_STENCIL
        };

        Texture(Type type = Type::Texture2D);
        ~Texture();

        // Prevent copying
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Allow moving
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        // Load texture from file
        bool LoadFromFile(const std::string& path);
        bool LoadCubemap(const std::vector<std::string>& faces);

        // Texture settings
        void SetFilterMode(FilterMode minFilter, FilterMode magFilter);
        void SetWrapMode(WrapMode sWrap, WrapMode tWrap, WrapMode rWrap = WrapMode::Repeat);
        void SetBorderColor(float r, float g, float b, float a);
        void GenerateMipmap();

        // Binding
        void Bind(unsigned int slot = 0) const;
        void Unbind() const;

        // Getters
        unsigned int GetID() const { return m_TextureID; }
        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }
        int GetChannels() const { return m_Channels; }
        Type GetType() const { return m_Type; }

    private:
        unsigned int m_TextureID;
        Type m_Type;
        int m_Width;
        int m_Height;
        int m_Channels;

        // Convert enum values to OpenGL constants
        static GLenum ConvertTypeToGL(Type type);
        static GLenum ConvertFilterToGL(FilterMode filter);
        static GLenum ConvertWrapToGL(WrapMode wrap);
        static GLenum ConvertFormatToGL(Format format, int channels);
    };
}

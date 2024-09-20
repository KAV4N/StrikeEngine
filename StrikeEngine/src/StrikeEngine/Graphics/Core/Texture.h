#pragma once

#define DEFAULT_TEXTURE "assets/textures/default.png"

#include <string>
#include <glad/glad.h>  

namespace StrikeEngine {



    class Texture
    {
    public:
        Texture(const std::string& path);
        ~Texture();

        enum TextureType {
            DIFFUSE,
            SPECULAR,
        };

        void Bind(unsigned int slot = 0);
        void Unbind();

        inline int GetWidth() const { return m_Width; }
        inline int GetHeight() const { return m_Height; }
        inline const std::string& GetPath() const { return m_Path; }
        inline void SetType(const std::string& type) { m_Type = type; }
        inline const std::string& GetType() const { return m_Type; }
        inline unsigned int GetTextureID() { return m_TextureID; }
    private:
        unsigned int m_TextureID;
        int m_Width, m_Height;
        unsigned int m_InternalFormat;
        unsigned int m_DataFormat;
        std::string m_Path;
        std::string m_Type; 

        void LoadTextureFromFile(const std::string& path);
    };

}

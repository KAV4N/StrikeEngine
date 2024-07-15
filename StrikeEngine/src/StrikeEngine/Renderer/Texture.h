#pragma once

#include <string>
#include <glad/glad.h>


namespace StrikeEngine {

    class Texture
    {
    public:
        Texture(unsigned int id = 0);
        Texture(const std::string& path);
        ~Texture();

        
        void Bind(unsigned int slot = 0) const;
        void Unbind() const; 

        inline unsigned int getWidth() const { return m_Width; }
        inline unsigned int getHeight() const { return m_Height; }
        inline unsigned int GetID() const { return m_TextureID; }

    private:
        unsigned int m_TextureID;
        unsigned int m_Width, m_Height;
        GLenum m_InternalFormat, m_DataFormat;

        void LoadTextureFromFile(const std::string& path);
    };

}

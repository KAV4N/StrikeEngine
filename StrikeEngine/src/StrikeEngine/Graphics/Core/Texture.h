#pragma once
#include <string>
#include <memory>
#include "TextureTypes.h"

namespace StrikeEngine {

    class Texture {
    public:
        ~Texture();

        void Bind() const;
        void SetData(const void* data, TextureTarget target = TextureTarget::AUTO) const;
        void SetParameter(TextureTarget target, TextureParameterName paramName, TextureParameter paramValue) const;
        void GenerateMipmap(TextureTarget target) const;

        unsigned int GetID() const { return m_TextureID; }
        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }
        TextureType GetType() const { return m_Type; }
        TextureUnit GetUnit() const { return m_Unit; }
        std::string GetPath() const { return m_Path; }

    private:
        friend class TextureManager;

        Texture(unsigned int id, TextureType type, TextureUnit unit,
            int width, int height, const std::string& path);

        unsigned int m_TextureID;
        int m_Width, m_Height;
        TextureType m_Type;
        TextureUnit m_Unit;
        std::string m_Path;
    };

} 
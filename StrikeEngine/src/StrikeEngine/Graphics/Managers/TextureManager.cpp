#include "strikepch.h"

#include "TextureManager.h"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>
#include <glm/ext/vector_float3.hpp>


namespace StrikeEngine {

    TextureManager* TextureManager::s_Instance = nullptr;

    TextureManager* TextureManager::Create() {
        if (!s_Instance) {
            s_Instance = new TextureManager();
        }
        return s_Instance;
    }

    TextureManager* TextureManager::Get() {
        return s_Instance;
    }

    std::shared_ptr<Texture> TextureManager::Load(const std::string& path, TextureType type,
        TextureUnit unit, bool flip) {
        auto it = m_Textures.find(path);
        if (it != m_Textures.end()) {
            return it->second;
        }

        int width, height, channels;
        uint8_t* data = LoadImage(path, width, height, channels, flip);

        unsigned int textureID = CreateGLTexture(type);
        ConfigureTexture(type, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
            ChannelsToFormat(channels), GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        auto texture = std::shared_ptr<Texture>(
            new Texture(textureID, type, unit, width, height, path)
        );
        m_Textures[path] = texture;
        return texture;
    }

    std::shared_ptr<Texture> TextureManager::CreateCubeMap(const std::string& directory,
        TextureUnit unit,
        const std::string& ext) {
        const std::string faces[6] = {
            "right", "left", "top", "bottom", "front", "back"
        };

        unsigned int textureID = CreateGLTexture(TextureType::CUBE_MAP);

        int width, height, channels;
        for (unsigned int i = 0; i < 6; i++) {
            std::string path = directory + "/" + faces[i] + ext;
            uint8_t* data = LoadImage(path, width, height, channels, false);

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }

        ConfigureTexture(TextureType::CUBE_MAP, textureID);

        auto texture = std::shared_ptr<Texture>(
            new Texture(textureID, TextureType::CUBE_MAP, unit, width, height, directory)
        );
        m_Textures[directory] = texture;
        return texture;
    }

    std::shared_ptr<Texture> TextureManager::CreateHDR(const std::string& path,
        TextureUnit unit) {
        int width, height, channels;
        float* data = LoadHDRImage(path, width, height, channels, true);

        unsigned int textureID = CreateGLTexture(TextureType::HDR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0,
            GL_RGB, GL_FLOAT, data);

        ConfigureTexture(TextureType::HDR, textureID);

        stbi_image_free(data);

        auto texture = std::shared_ptr<Texture>(
            new Texture(textureID, TextureType::HDR, unit, width, height, path)
        );
        m_Textures[path] = texture;
        return texture;
    }

    void TextureManager::Unload(const std::string& key) {
        m_Textures.erase(key);
    }

    unsigned int TextureManager::CreateGLTexture(TextureType type) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(type == TextureType::CUBE_MAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, textureID);
        return textureID;
    }

    void TextureManager::ConfigureTexture(TextureType type, unsigned int textureID) {
        GLenum target = type == TextureType::CUBE_MAP ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;

        if (type == TextureType::DIFFUSE) {
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else if (type == TextureType::CUBE_MAP) {
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }
        else if (type == TextureType::HDR) {
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
    }

    uint8_t* TextureManager::LoadImage(const std::string& path, int& width, int& height,
        int& channels, bool flip) {
        stbi_set_flip_vertically_on_load(flip);
        uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (!data) {
            throw std::runtime_error("Failed to load texture: " + path);
        }
        return data;
    }

    float* TextureManager::LoadHDRImage(const std::string& path, int& width, int& height,
        int& channels, bool flip) {
        stbi_set_flip_vertically_on_load(flip);
        float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
        if (!data) {
            throw std::runtime_error("Failed to load HDR texture: " + path);
        }
        return data;
    }

    TextureFormat TextureManager::ChannelsToFormat(int channels) {
        switch (channels) {
        case 1: return TextureFormat::R;
        case 3: return TextureFormat::RGB;
        case 4: return TextureFormat::RGBA;
        default: throw std::runtime_error("Unsupported number of texture channels");
        }
    }
} 
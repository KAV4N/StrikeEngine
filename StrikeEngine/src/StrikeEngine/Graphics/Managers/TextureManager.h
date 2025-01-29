#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "StrikeEngine/Graphics/Core/Texture.h"
#include <glm/ext/vector_float3.hpp>

namespace StrikeEngine {

    class TextureManager {
    public:
        static TextureManager* Create();
        static TextureManager* Get();

        std::shared_ptr<Texture> Load(const std::string& path, TextureType type,
            TextureUnit unit = TEX_0, bool flip = true);

        std::shared_ptr<Texture> CreateCubeMap(const std::string& directory,
            TextureUnit unit = TEX_0,
            const std::string& ext = ".jpg");

        std::shared_ptr<Texture> CreateHDR(const std::string& path,
            TextureUnit unit = TEX_0);

        void Unload(const std::string& key);
    private:
        TextureManager() = default;
        ~TextureManager() = default;

        unsigned int CreateGLTexture(TextureType type);
        void ConfigureTexture(TextureType type, unsigned int textureID);
        uint8_t* LoadImage(const std::string& path, int& width, int& height, int& channels, bool flip);
        float* LoadHDRImage(const std::string& path, int& width, int& height, int& channels, bool flip);
        TextureFormat ChannelsToFormat(int channels);

        static TextureManager* s_Instance;
        std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
    };

} 
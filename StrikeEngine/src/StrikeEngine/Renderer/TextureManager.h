#pragma once

#include <unordered_map>
#include <string>
#include "Texture.h" 

namespace StrikeEngine {

    class TextureManager
    {
    public:
        static TextureManager* Get(); 

        static void Create();

        Texture* LoadTexture(const std::string& name, const std::string& path);

        Texture* GetTexture(const std::string& name);

        void ReleaseTexture(const std::string& name);

        void ReleaseAllTextures();

    private:
        TextureManager();
        ~TextureManager(); 
        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;

        std::unordered_map<std::string, Texture*> m_Textures;
        static TextureManager* s_Instance; 
    };

} 


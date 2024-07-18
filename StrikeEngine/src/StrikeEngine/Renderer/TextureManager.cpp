#include "strikepch.h"
#include "TextureManager.h"

namespace StrikeEngine {

    TextureManager* TextureManager::s_Instance = nullptr; 


    TextureManager* TextureManager::Get() 
    {
        return s_Instance;
    }

    void TextureManager::Create()
    {
        if (!s_Instance) 
        {
            s_Instance = new TextureManager();
        }
    }

    Texture* TextureManager::LoadTexture(const std::string& name, const std::string& path)
    {
        m_Textures[name] = new Texture(path);
        return m_Textures[name];
 
    }

    Texture* TextureManager::GetTexture(const std::string& name)
    {
        auto it = m_Textures.find(name);
        if (it != m_Textures.end()) {
            return it->second;
        }
        return nullptr;
    }

    void TextureManager::ReleaseTexture(const std::string& name)
    {
        auto it = m_Textures.find(name);
        if (it != m_Textures.end()) {
            delete it->second;
            m_Textures.erase(it);
        }
    }

    void TextureManager::ReleaseAllTextures()
    {
        for (auto& pair : m_Textures) {
            delete pair.second;
        }
        m_Textures.clear();
    }

    TextureManager::TextureManager()
    {
    }

    TextureManager::~TextureManager()
    {
        ReleaseAllTextures();
    }

} 


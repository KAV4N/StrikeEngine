

#include "strikepch.h"
#include "MaterialManager.h"
#include "StrikeEngine/Graphics/Core/Shader.h"
#include "StrikeEngine/Graphics/Core/Material.h"
#include "StrikeEngine/Graphics/Managers/TextureManager.h"
#include "StrikeEngine/Graphics/Managers/ShaderManager.h"


namespace StrikeEngine {

    MaterialManager* MaterialManager::s_Instance = nullptr;


    MaterialManager* MaterialManager::Create() {
        if (s_Instance == nullptr) {
            s_Instance = new MaterialManager();

        }
        return s_Instance;
    }


    MaterialManager* MaterialManager::Get() {
        if (s_Instance == nullptr) {
            std::cerr << "MaterialManager not created. Call Create() first." << std::endl;
            return nullptr;
        }
        return s_Instance;
    }

    MaterialManager::~MaterialManager() {
        Clear();
        s_Instance = nullptr;
    }

    std::shared_ptr<Material> MaterialManager::CreateMaterial(std::shared_ptr<Shader> shader, const std::string& name) {
        if (HasMaterial(name)) {
            std::cerr << "Material with name '" << name << "' already exists. Returning existing material." << std::endl;
            return GetMaterial(name);
        }
        if (!shader) {
            std::cerr << "Cannot create material with null shader" << std::endl;
            return nullptr;
        }

        try {
            auto material = std::make_shared<Material>(shader);

            m_Materials[name] = material;

            return material;
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to create material '" << name << "': " << e.what() << std::endl;
            return nullptr;
        }
    }

    std::shared_ptr<Material> MaterialManager::GetMaterial(const std::string& name) {
        auto it = m_Materials.find(name);
        if (it != m_Materials.end()) {
            return it->second;
        }

        std::cerr << "Material with name '" << name << "' not found." << std::endl;
        return nullptr;
    }

    void MaterialManager::RemoveMaterial(const std::string& name) {
        auto it = m_Materials.find(name);
        if (it != m_Materials.end()) {
            m_Materials.erase(it);
        }
        else {
            std::cerr << "Material with name '" << name << "' not found." << std::endl;
        }
    }



    std::shared_ptr<Material> MaterialManager::GetDefaultMaterial() {
        if (!m_DefaultMaterial) {
            auto shaderManager = ShaderManager::Get();
        }
        return m_DefaultMaterial;
    }


    bool MaterialManager::HasMaterial(const std::string& name) const {
        return m_Materials.find(name) != m_Materials.end();
    }

    void MaterialManager::Clear() {
        m_Materials.clear();
    }
}

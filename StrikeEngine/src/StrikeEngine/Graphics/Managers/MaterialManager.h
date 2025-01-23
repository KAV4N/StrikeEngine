#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <iostream>

namespace StrikeEngine {
    class Material;
    class Shader;

    class MaterialManager {
    public:
        MaterialManager(const MaterialManager&) = delete;
        MaterialManager& operator=(const MaterialManager&) = delete;

        static MaterialManager* Create();
        static MaterialManager* Get();

        std::shared_ptr<Material> CreateMaterial(std::shared_ptr<Shader> shader, const std::string& name);
        std::shared_ptr<Material> GetMaterial(const std::string& name);
        void RemoveMaterial(const std::string& name);
        bool HasMaterial(const std::string& name) const;
        void Clear();

    private:
        MaterialManager() = default;
        ~MaterialManager();

        static MaterialManager* s_Instance;
        std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;
    };
}

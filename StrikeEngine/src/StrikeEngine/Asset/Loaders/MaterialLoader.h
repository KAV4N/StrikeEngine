#pragma once

#include <pugixml.hpp>
#include <filesystem>
#include <memory>
#include <string>

#include "StrikeEngine/Asset/Loaders/AssetLoader.h"
#include "StrikeEngine/Asset/Types/Material.h"

namespace StrikeEngine {

    class MaterialLoader : public AssetLoader {
    public:
        MaterialLoader();

        std::shared_ptr<Asset> load(const std::string& id, const std::filesystem::path& path, bool async = false) override;
        std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) override;

    protected:
        std::shared_ptr<Asset> createPlaceholder(const std::string& id, const std::filesystem::path& path) override;
        void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) override;

    private:
        void loadMaterialFromXml(std::shared_ptr<Material> material, const pugi::xml_node& materialNode, const std::filesystem::path& basePath);
        void loadProperties(std::shared_ptr<Material> material, const pugi::xml_node& propertiesNode);
        void loadTextures(std::shared_ptr<Material> material, const pugi::xml_node& texturesNode, const std::filesystem::path& basePath);
        glm::vec3 parseVec3(const std::string& value);
        glm::vec4 parseVec4(const std::string& value);
    };

}
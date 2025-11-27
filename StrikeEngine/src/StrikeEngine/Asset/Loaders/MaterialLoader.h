#pragma once

#include "AssetLoader.h"
#include <pugixml.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <glm/glm.hpp>


namespace StrikeEngine {

    class Asset;
    class Material;

    class MaterialLoader : public AssetLoader {
    public:
        MaterialLoader();

        std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) override;

    protected:
        std::shared_ptr<Asset> loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async=false) override;
        std::shared_ptr<Asset> createPlaceholder(const std::string& id, const std::filesystem::path& path) override;
        void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) override;

    private:
        void loadMaterialFromXml(std::shared_ptr<Material> material, const pugi::xml_node& materialNode, const std::filesystem::path& basePath);
        void loadProperties(std::shared_ptr<Material> material, const pugi::xml_node& materialNode);
        void loadTextures(std::shared_ptr<Material> material, const pugi::xml_node& materialNode, const std::filesystem::path& basePath);
        glm::ivec3 parseVec3(const pugi::xml_node& node);
    };

}
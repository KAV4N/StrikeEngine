#pragma once

#include <pugixml.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include "StrikeEngine/Asset/Loaders/AssetLoader.h"

namespace StrikeEngine {

    class Asset;
    class Template;

    class TemplateLoader : public AssetLoader {
    public:
        TemplateLoader();

        std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) override;

    protected:
        std::shared_ptr<Asset> loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async=false) override;
        std::shared_ptr<Asset> createPlaceholder(const std::string& id, const std::filesystem::path& path) override;
        void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) override;

    private:
        void loadAssets(std::shared_ptr<Template> templateAsset, 
                        const pugi::xml_node& assetsNode, 
                        const std::filesystem::path& basePath
                        );
    };

}
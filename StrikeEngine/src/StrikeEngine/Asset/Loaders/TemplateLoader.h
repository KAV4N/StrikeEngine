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

        /**
         * @brief Load an asset from an XML node.
         * @param node The XML node containing asset data.
         * @param basePath The base path to resolve relative asset paths.
         * @return Shared pointer to the loaded asset.
         */
        std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) override;

    protected:
        std::shared_ptr<Asset> loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async=false) override;
        void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) override;

    private:
        void loadAssets(std::shared_ptr<Template> templateAsset, 
                        const pugi::xml_node& assetsNode, 
                        const std::filesystem::path& basePath
                        );
    };

}
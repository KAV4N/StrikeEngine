#pragma once

#include <pugixml.hpp>
#include <filesystem>
#include <memory>
#include <string>

#include "StrikeEngine/Asset/Loaders/AssetLoader.h"
#include "StrikeEngine/Asset/Types/Template.h"

namespace StrikeEngine {

    class TemplateLoader : public AssetLoader {
    public:
        TemplateLoader();

        std::shared_ptr<Asset> load(const std::string& id, const std::filesystem::path& path, bool async) override;
        std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node) override;
        void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) override;
    protected:
        std::shared_ptr<Asset> createPlaceholder(const std::string& id, const std::filesystem::path& path) override;
    };

}
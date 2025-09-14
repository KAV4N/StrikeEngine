#pragma once

#include "StrikeEngine/Asset/Loaders/AssetLoader.h"
#include "StrikeEngine/Asset/Types/Texture2D.h"
#include <pugixml.hpp>
#include <filesystem>
#include <memory>
#include <string>

namespace StrikeEngine {

    class Texture2DLoader : public AssetLoader {
    public:
        Texture2DLoader();

        std::shared_ptr<Asset> load(const std::string& id, const std::filesystem::path& path, bool async) override;
        std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) override;
        void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) override;

    protected:
        std::shared_ptr<Asset> createPlaceholder(const std::string& id, const std::filesystem::path& path) override;

    private:
        unsigned char* loadImageData(const std::filesystem::path& path, int& width, int& height, int& channels);
    };

}
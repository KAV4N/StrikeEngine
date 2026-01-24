#pragma once

#include "AssetLoader.h"
#include <pugixml.hpp>
#include <filesystem>
#include <memory>
#include <string>

namespace StrikeEngine {

    class Asset;
    class Audio;

    class AudioLoader : public AssetLoader {
    public:
        AudioLoader();

        std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) override;

    protected:
        std::shared_ptr<Asset> loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async = false) override;
        void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) override;

    private:
        std::shared_ptr<Audio> loadAudioFile(const std::string& id, const std::filesystem::path& path);
    };

}
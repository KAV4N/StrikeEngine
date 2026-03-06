#pragma once

#include "AssetLoader.h"
#include <pugixml.hpp>
#include <filesystem>
#include <memory>
#include <string>

namespace Strike {

    class Asset;
    class Audio;

    class AudioLoader : public AssetLoader {
    public:
        AudioLoader();

        /**
         * @brief Load an asset from an XML node.
         * @param node The XML node containing asset data.
         * @param basePath The base path to resolve relative asset paths.
         * @return Shared pointer to the loaded asset.
         */
        std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) override;

    protected:
        std::shared_ptr<Asset> loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async = false) override;
        void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) override;

    private:
        std::shared_ptr<Audio> loadAudioFile(const std::string& id, const std::filesystem::path& path);

        /**
         * @brief Bakes a downsampled RMS amplitude envelope into the Audio asset.
         * Called once at load time. Zero runtime cost during playback.
         * @param asset The audio asset to bake into.
         * @param path  File path of the audio file.
         */
        void bakeAmplitudeEnvelope(Audio& asset, const std::filesystem::path& path);
    };

}
#include "strikepch.h"
#include "AudioLoader.h"
#include "StrikeEngine/Asset/Types/Audio.h"
#include "StrikeEngine/Asset/AssetManager.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace Strike {

    AudioLoader::AudioLoader() : AssetLoader(Audio::getStaticTypeName()) {
    }

    std::shared_ptr<Asset> AudioLoader::loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async) {
        auto audio = loadAudioFile(id, path);
        return audio;
    }

    void AudioLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderAudio = std::dynamic_pointer_cast<Audio>(placeholder);
        auto loadedAudio = std::dynamic_pointer_cast<Audio>(loaded);
        
        if (placeholderAudio && loadedAudio) {
            *placeholderAudio = std::move(*loadedAudio);
        }
    }

    std::shared_ptr<Audio> AudioLoader::loadAudioFile(const std::string& id, const std::filesystem::path& path) {
        auto asset = std::make_shared<Audio>(id, path);
        asset->setState(AssetState::Loading);

        if (!std::filesystem::exists(path)) {
            STRIKE_CORE_ERROR("Audio file does not exist: {}", path.string());
            asset->setState(AssetState::Failed);
            return asset;
        }

        ma_decoder decoder;
        if (ma_decoder_init_file(path.string().c_str(), nullptr, &decoder) != MA_SUCCESS) {
            STRIKE_CORE_ERROR("Failed to initialize audio decoder for file: {}", path.string());
            asset->setState(AssetState::Failed);
            return asset;
        }

        // Get total frame count for duration calculation
        ma_uint64 frameCount = 0;
        if (ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount) != MA_SUCCESS) {
            STRIKE_CORE_ERROR("Failed to get audio length for file: {}", path.string());
            ma_decoder_uninit(&decoder);
            asset->setState(AssetState::Failed);
            return asset;
        }

        ma_uint32 sampleRate = decoder.outputSampleRate;
        asset->mDuration = static_cast<float>(frameCount) / static_cast<float>(sampleRate);
        
        ma_decoder_uninit(&decoder);

        return asset;
    }

    std::shared_ptr<Asset> AudioLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        return loadFromNodeInternal<Audio>(node, basePath);
    }

}
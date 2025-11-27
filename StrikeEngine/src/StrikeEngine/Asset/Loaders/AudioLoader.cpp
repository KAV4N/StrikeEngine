#include "AudioLoader.h"
#include "StrikeEngine/Asset/Types/Audio.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include <stdexcept>
#include <iostream>

#include <miniaudio.h>

namespace StrikeEngine {

    AudioLoader::AudioLoader() : AssetLoader(Audio::getStaticTypeName()) {
    }

    std::shared_ptr<Asset> AudioLoader::loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async) {
        auto audio = loadAudioFile(id, path);
        
        if (!audio) {
            throw std::runtime_error("Failed to load audio file: " + path.string());
        }

        return audio;
    }

    std::shared_ptr<Asset> AudioLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        std::string assetId = node.attribute("id").as_string();
        std::filesystem::path src = node.attribute("src").as_string();

        src = resolvePath(src, basePath);

        if (assetId.empty() || src.empty()) {
            throw std::runtime_error("Invalid audio node: missing id or src attribute");
        }

        bool async = node.attribute("async").as_bool();
        if (async)
            return AssetManager::get().loadAudioAsync(assetId, src);
        else
            return AssetManager::get().loadAudio(assetId, src);
    }

    std::shared_ptr<Asset> AudioLoader::createPlaceholder(const std::string& id, const std::filesystem::path& path) {
        return std::make_shared<Audio>(id, path);
    }

    void AudioLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderAudio = std::dynamic_pointer_cast<Audio>(placeholder);
        auto loadedAudio = std::dynamic_pointer_cast<Audio>(loaded);
        
        if (placeholderAudio && loadedAudio) {
            *placeholderAudio = std::move(*loadedAudio);
        }
    }

    std::shared_ptr<Audio> AudioLoader::loadAudioFile(const std::string& id, const std::filesystem::path& path) {
        ma_decoder decoder;
        ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 0, 0);
        
        if (ma_decoder_init_file(path.string().c_str(), &config, &decoder) != MA_SUCCESS) {
            std::cerr << "Failed to initialize miniaudio decoder for: " << path.string() << std::endl;
            return nullptr;
        }

        // Get audio info
        ma_uint64 frameCount = 0;
        ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount);

        // Calculate buffer size
        ma_uint64 totalSamples = frameCount * decoder.outputChannels;
        size_t bufferSize = totalSamples * ma_get_bytes_per_frame(decoder.outputFormat, decoder.outputChannels);

        // Allocate buffer and decode entire audio
        std::vector<float> audioData(bufferSize);
        ma_uint64 framesRead = 0;
        
        if (ma_decoder_read_pcm_frames(&decoder, audioData.data(), frameCount, &framesRead) != MA_SUCCESS) {
            ma_decoder_uninit(&decoder);
            std::cerr << "Failed to decode audio data for: " << path.string() << std::endl;
            return nullptr;
        }

        // Resize to actual data read
        audioData.resize(framesRead * decoder.outputChannels * ma_get_bytes_per_frame(decoder.outputFormat, decoder.outputChannels));

        // Create Audio asset
        auto audio = std::make_shared<Audio>(id, addRootPrefix(path));
        
        // Fill AudioData struct
        auto& newAudioData = audio->getAudioData();

        newAudioData.data = std::move(audioData);
        newAudioData.sampleRate = decoder.outputSampleRate;
        newAudioData.channels = decoder.outputChannels;
        newAudioData.format = decoder.outputFormat;
        newAudioData.frameCount = framesRead;

        ma_decoder_uninit(&decoder);

        return audio;
    }

}
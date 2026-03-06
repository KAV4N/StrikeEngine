#include "strikepch.h"
#include "AudioLoader.h"
#include "StrikeEngine/Asset/Types/Audio.h"
#include "StrikeEngine/Asset/AssetManager.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace Strike {

    // Number of PCM frames per envelope hop.
    static constexpr uint32_t kEnvelopeHopFrames = 512;
    static constexpr uint32_t kEnvelopeChannels  = 2;

    AudioLoader::AudioLoader() : AssetLoader(Audio::getStaticTypeName()) {
    }

    std::shared_ptr<Asset> AudioLoader::loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async) {
        return loadAudioFile(id, path);
    }

    void AudioLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderAudio = std::dynamic_pointer_cast<Audio>(placeholder);
        auto loadedAudio      = std::dynamic_pointer_cast<Audio>(loaded);

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


        {
            ma_decoder decoder;
            if (ma_decoder_init_file(path.string().c_str(), nullptr, &decoder) != MA_SUCCESS) {
                STRIKE_CORE_ERROR("Failed to initialize audio decoder for file: {}", path.string());
                asset->setState(AssetState::Failed);
                return asset;
            }

            ma_uint64 frameCount = 0;
            if (ma_decoder_get_length_in_pcm_frames(&decoder, &frameCount) != MA_SUCCESS) {
                STRIKE_CORE_ERROR("Failed to get audio length for file: {}", path.string());
                ma_decoder_uninit(&decoder);
                asset->setState(AssetState::Failed);
                return asset;
            }

            const ma_uint32 sampleRate = decoder.outputSampleRate;
            asset->mDuration = static_cast<float>(frameCount) / static_cast<float>(sampleRate);

            ma_decoder_uninit(&decoder);
        }

        bakeAmplitudeEnvelope(*asset, path);

        asset->setState(AssetState::Ready);
        return asset;
    }

    void AudioLoader::bakeAmplitudeEnvelope(Audio& asset, const std::filesystem::path& path) {
        ma_decoder_config cfg = ma_decoder_config_init(ma_format_f32, kEnvelopeChannels, 0);

        ma_decoder decoder;
        if (ma_decoder_init_file(path.string().c_str(), &cfg, &decoder) != MA_SUCCESS) {
            STRIKE_CORE_ERROR("Amplitude envelope: failed to open decoder for: {}", path.string());
            return;
        }

        const float sampleRate = static_cast<float>(decoder.outputSampleRate);
        asset.mEnvelopeFrameDuration = static_cast<float>(kEnvelopeHopFrames) / sampleRate;

        std::vector<float> hop(kEnvelopeHopFrames * kEnvelopeChannels, 0.0f);

        while (true) {
            ma_uint64 framesRead = 0;
            const ma_result result = ma_decoder_read_pcm_frames(&decoder, hop.data(), kEnvelopeHopFrames, &framesRead);

            if (framesRead == 0)
                break;

            float sumSquares = 0.0f;
            const uint64_t totalSamples = framesRead * kEnvelopeChannels;
            for (uint64_t i = 0; i < totalSamples; ++i)
                sumSquares += hop[i] * hop[i];

            asset.mAmplitudeEnvelope.push_back(
                std::sqrt(sumSquares / static_cast<float>(totalSamples))
            );

            if (result == MA_AT_END)
                break;
        }

        ma_decoder_uninit(&decoder);
    }

    std::shared_ptr<Asset> AudioLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        return loadFromNodeInternal<Audio>(node, basePath);
    }

}
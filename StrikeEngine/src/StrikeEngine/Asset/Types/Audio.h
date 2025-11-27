#pragma once
#include "Asset.h"
#include <vector>
#include <cstdint>
#include <miniaudio.h>

namespace StrikeEngine {

    struct AudioData {
        std::vector<float> data;
        uint32_t sampleRate;
        uint32_t channels;
        ma_format format;
        uint64_t frameCount;

        AudioData()
            : sampleRate(0),
            channels(0),
            format(ma_format_unknown),
            frameCount(0)
        {
        }

        float getDuration() const {
            if (sampleRate == 0) {
                return 0.0f;
            }
            return static_cast<float>(frameCount) / static_cast<float>(sampleRate);
        }
    };

    class Audio : public Asset {
    public:
        Audio(const std::string& id, const std::filesystem::path& path);
        ~Audio();

        static const std::string& getStaticTypeName();
        const std::string& getTypeName() const override;

        // Audio data access
        const AudioData& getAudioData() const;
        AudioData& getAudioData();
        
        // Convenience getters
        const std::vector<float>& getData() const;
        uint32_t getSampleRate() const;
        uint32_t getChannels() const;
        ma_format getFormat() const;
        uint64_t getFrameCount() const;
        float getDuration() const;

    private:
        AudioData mAudioData;
    };

}
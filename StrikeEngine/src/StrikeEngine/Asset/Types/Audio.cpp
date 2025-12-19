#include "strikepch.h"
#include "Audio.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace StrikeEngine {

    Audio::Audio(const std::string& id, const std::filesystem::path& path)
        : Asset(id, path)
    {
    }

    Audio::~Audio() {
        mAudioData.data.clear();
    }

    const std::string& Audio::getStaticTypeName() {
        static const std::string typeName = "audio";
        return typeName;
    }

    const std::string& Audio::getTypeName() const {
        return getStaticTypeName();
    }

    const AudioData& Audio::getAudioData() const {
        return mAudioData;
    }

    AudioData& Audio::getAudioData() {
        return mAudioData;
    }

    const std::vector<float>& Audio::getData() const {
        return mAudioData.data;
    }

    uint32_t Audio::getSampleRate() const {
        return mAudioData.sampleRate;
    }

    uint32_t Audio::getChannels() const {
        return mAudioData.channels;
    }

    ma_format Audio::getFormat() const {
        return mAudioData.format;
    }

    uint64_t Audio::getFrameCount() const {
        return mAudioData.frameCount;
    }

    float Audio::getDuration() const {
        return mAudioData.getDuration();
    }
}
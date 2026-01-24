#include "strikepch.h"
#include "AudioSourceComponent.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "StrikeEngine/Asset/Types/Audio.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

namespace StrikeEngine {

    REGISTER_COMPONENT(AudioSourceComponent);


    AudioSourceComponent::AudioSourceComponent() {
    }

    void AudioSourceComponent::deserialize(const pugi::xml_node& node) {
        mIsActive = node.attribute("active").as_bool(true);
        
        std::string audioId = node.attribute("audio").as_string();
        if (!audioId.empty()) {
            setAudio(audioId);
        }
        
        mVolume = node.attribute("volume").as_float(1.0f);
        mLoop = node.attribute("loop").as_bool(false);
        mSpatial = node.attribute("spatial").as_bool(false);
        minDistance = node.attribute("minDistance").as_float(1.0f);
        maxDistance = node.attribute("maxDistance").as_float(50.0f);
    }

    
    void AudioSourceComponent::setAudio(const std::string& audioId) {
        mAudioId = audioId;
    }

    void AudioSourceComponent::removeAudio() {
        stop();
        mAudioId.clear();
    }

    std::shared_ptr<Audio> AudioSourceComponent::getAudio() const {
        if (mAudioId.empty()) {
            return nullptr;
        }
        return AssetManager::get().getAsset<Audio>(mAudioId);
    }

    bool AudioSourceComponent::hasAudio() const {
        return !mAudioId.empty() && getAudio() != nullptr;
    }

    void AudioSourceComponent::play() {
        if (!hasAudio()) {
            return;
        }
        mPlayRequested = true;
    }

    void AudioSourceComponent::stop() {
        mStopRequested = true;
    }

    void AudioSourceComponent::setVolume(float volume) {
        mVolume = glm::clamp(volume, 0.0f, 1.0f);
    }

}
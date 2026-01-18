#include "strikepch.h"
#include "AudioSourceComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "StrikeEngine/Asset/Types/Audio.h"

namespace StrikeEngine {
    REGISTER_COMPONENT(AudioSourceComponent)

    AudioSourceComponent::AudioSourceComponent() {}

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
        return !mAudioId.empty();
    }

    void AudioSourceComponent::play() {
        if (mAudioId.empty()) return;
        mIsPlaying = true;
        mIsPaused = false;
    }

    void AudioSourceComponent::pause() {
        if (!mIsPlaying) return;
        mIsPaused = true;
    }

    void AudioSourceComponent::stop() {
        mIsPlaying = false;
        mIsPaused = false;
        mCurrentFrame = 0;
    }

    void AudioSourceComponent::setVolume(float volume) {
        mVolume = glm::clamp(volume, 0.0f, 1.0f);
    }

    void AudioSourceComponent::deserialize(const pugi::xml_node& node) {
        if (auto attr = node.attribute("audio")) {
            mAudioId = attr.as_string();
        }

        if (auto attr = node.attribute("volume")) {
            setVolume(attr.as_float());
        }

        if (auto attr = node.attribute("loop")) {
            setLoop(attr.as_bool());
        }

        if (auto attr = node.attribute("autoplay")) {
            setAutoplay(attr.as_bool());
        }

        if (auto attr = node.attribute("spatial")) {
            setSpatial(attr.as_bool());
        }

        if (auto attr = node.attribute("active")) {
            setActive(attr.as_bool(true));
        }
    }

    void AudioSourceComponent::serialize(pugi::xml_node& node) const {
        if (!mAudioId.empty()) {
            node.append_attribute("audio") = mAudioId.c_str();
        }

        node.append_attribute("volume") = mVolume;
        node.append_attribute("loop") = mLoop;
        node.append_attribute("autoplay") = mAutoplay;
        node.append_attribute("spatial") = mSpatial;

        node.append_attribute("active") = isActive();
    }
}
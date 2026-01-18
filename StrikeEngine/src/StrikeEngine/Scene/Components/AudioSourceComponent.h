#pragma once
#include "Component.h"
#include <string>
#include <memory>
#include <pugixml.hpp>

namespace StrikeEngine {
    class Audio;

    class AudioSourceComponent : public Component {
    public:
        AudioSourceComponent();

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "audiosource";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // Audio clip management
        void setAudio(const std::string& audioId);
        void removeAudio();
        const std::string& getAudioId() const { return mAudioId; }
        std::shared_ptr<Audio> getAudio() const;
        bool hasAudio() const;

        // Playback controls
        void play();
        void pause();
        void stop();
        bool isPlaying() const { return mIsPlaying; }
        bool isPaused() const { return mIsPaused; }

        // Properties
        void setVolume(float volume);
        float getVolume() const { return mVolume; }

        void setLoop(bool loop) { mLoop = loop; }
        bool isLoop() const { return mLoop; }

        void setAutoplay(bool autoplay) { mAutoplay = autoplay; }
        bool isAutoplay() const { return mAutoplay; }

        void setSpatial(bool spatial) { mSpatial = spatial; }
        bool isSpatial() const { return mSpatial; }

        // Playback position
        void setPlaybackPosition(uint64_t frame) { mCurrentFrame = frame; }
        uint64_t getPlaybackPosition() const { return mCurrentFrame; }

    private:
        friend class AudioSystem;

        std::string mAudioId;
        float mVolume = 1.0f;
        bool mLoop = false;
        bool mAutoplay = false;
        bool mSpatial = true;
        bool mIsPlaying = false;
        bool mIsPaused = false;
        uint64_t mCurrentFrame = 0;
    };
}
#pragma once
#include "Component.h"
#include <string>
#include <memory>
#include <vector>
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

        // Audio clip management
        void setAudio(const std::string& audioId);
        void removeAudio();
        const std::string& getAudioId() const { return mAudioId; }
        std::shared_ptr<Audio> getAudio() const;
        bool hasAudio() const;

        void play(); 
        void stop(); 
    
        void setVolume(float volume);
        float getVolume() const { return mVolume; }

        void setLoop(bool loop) { mLoop = loop; }
        bool isLoop() const { return mLoop; }

        void setSpatial(bool spatial) { mSpatial = spatial; }
        bool isSpatial() const { return mSpatial; }

        void setMinDistance(float distance) { minDistance = distance; }
        float getMinDistance() const { return minDistance; }

        void setMaxDistance(float distance) { maxDistance = distance; }
        float getMaxDistance() const { return maxDistance; }

    private:
        friend class AudioSystem;

        std::string mAudioId;
        float mVolume = 1.0f;
        bool mLoop = false;
        bool mSpatial = false;

        float minDistance = 1.0f;
        float maxDistance = 50.0f;

        bool mPlayRequested = false;
        bool mStopRequested = false;
    };
}
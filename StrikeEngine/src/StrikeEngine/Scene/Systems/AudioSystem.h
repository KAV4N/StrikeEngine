#pragma once

#include "SystemECS.h"
#include "StrikeEngine/Asset/Types/Audio.h"

#include <miniaudio.h>
#include <unordered_map>
#include <vector>
#include <memory>

namespace StrikeEngine {

    class Scene;
    class AudioAsset;

    struct PlayingSound {
        std::unique_ptr<ma_sound> sound;
        bool isSpatial;
        float cachedVolume;
    };

    class AudioSystem : public SystemECS {
    public:
        AudioSystem();
        ~AudioSystem();

        bool initialize();
        void shutdown();

        void onUpdate(float dt) override;

        void stopAll();
        void stopEntity(entt::entity entity);

        void setMasterVolume(float volume);
        float getMasterVolume() const;

    private:
        void updateListener();
        void updateSources();
        void cleanupFinishedSounds();
        void processPlayRequests();
        void processStopRequests();

        ma_engine mEngine;
        bool mInitialized;
        float mMasterVolume;

        // Multiple sounds can play per entity (overlapping)
        std::unordered_map<entt::entity, std::vector<PlayingSound>> mPlayingSounds;
    };

} // namespace StrikeEngine
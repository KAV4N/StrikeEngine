#pragma once

#include "SystemECS.h"
#include "StrikeEngine/Asset/Types/Audio.h"

#include <miniaudio.h>
#include <unordered_map>
#include <vector>
#include <memory>

namespace Strike {

    class Scene;

    /**
     * @brief Runtime representation of a playing sound instance.
     *
     * Multiple PlayingSound instances may exist per entity to allow
     * overlapping playback.
     */
    struct PlayingSound {
        std::unique_ptr<ma_sound>  sound;          /**< Miniaudio sound instance */
        std::shared_ptr<Audio>     audioAsset;     /**< Cached asset - avoids per-frame AssetManager lookup */
        bool                       isSpatial    = false;
        float                      cachedVolume = 1.0f;
    };

    /**
     * @brief ECS system responsible for audio playback and spatialization.
     */
    class AudioSystem : public SystemECS {
    public:
        AudioSystem();
        ~AudioSystem();

        bool initialize();
        void shutdown();

        /**
         * @brief Updates audio playback and spatial state each frame.
         * @param dt Delta time in seconds.
         */
        void onUpdate(float dt) override;

        void stopAll();
        void stopEntity(entt::entity entity);

        void  setMasterVolume(float volume);
        float getMasterVolume() const;

        /**
         * @brief Returns the RMS amplitude [0.0, 1.0] of the first playing
         *        sound on the given entity.
         *
         * @param entity The EnTT entity handle owning the AudioSourceComponent.
         */
        float getAmplitude(entt::entity entity) const;

    private:
        void updateListener();
        void updateSources();
        void cleanupFinishedSounds();
        void processPlayRequests();
        void processStopRequests();

    private:
        ma_engine  mEngine;
        bool       mInitialized;
        float      mMasterVolume;

        std::unordered_map<entt::entity, std::vector<PlayingSound>> mPlayingSounds;
    };

}
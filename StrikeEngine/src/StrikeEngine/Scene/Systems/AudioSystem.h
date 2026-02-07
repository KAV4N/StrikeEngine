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

    /**
     * @brief Runtime representation of a playing sound instance.
     *
     * Multiple PlayingSound instances may exist per entity to allow
     * overlapping playback.
     */
    struct PlayingSound {
        std::unique_ptr<ma_sound> sound; /**< Miniaudio sound instance */
        bool isSpatial;                  /**< Whether the sound is spatialized */
        float cachedVolume;              /**< Cached volume to detect changes */
    };

    /**
     * @brief ECS system responsible for audio playback and spatialization.
     *
     * AudioSystem integrates the miniaudio engine with the ECS layer.
     * It manages audio listeners, audio sources, playback requests,
     * spatial updates, and lifetime cleanup of playing sounds.
     */
    class AudioSystem : public SystemECS {
    public:
        /**
         * @brief Constructs the audio system and initializes the audio engine.
         */
        AudioSystem();

        /**
         * @brief Shuts down the audio system and releases all resources.
         */
        ~AudioSystem();

        /**
         * @brief Initializes the miniaudio engine.
         *
         * Safe to call multiple times.
         *
         * @return True if initialization succeeded.
         */
        bool initialize();

        /**
         * @brief Shuts down the audio engine and stops all sounds.
         */
        void shutdown();

        /**
         * @brief Updates audio playback and spatial state.
         *
         * Handles listener updates, play/stop requests,
         * source synchronization, and cleanup of finished sounds.
         *
         * @param dt Delta time (in seconds).
         */
        void onUpdate(float dt) override;

        /**
         * @brief Stops all currently playing sounds.
         */
        void stopAll();

        /**
         * @brief Stops all sounds associated with an entity.
         *
         * @param entity Entity whose sounds should be stopped.
         */
        void stopEntity(entt::entity entity);

        /**
         * @brief Sets the global master volume.
         *
         * @param volume Volume in range [0.0, 1.0].
         */
        void setMasterVolume(float volume);

        /**
         * @brief Returns the current master volume.
         */
        float getMasterVolume() const;

    private:
        /**
         * @brief Updates the active audio listener transform.
         *
         * Uses the first active AudioListenerComponent found in the scene.
         */
        void updateListener();

        /**
         * @brief Updates all playing audio sources.
         *
         * Synchronizes volume and spatial position with entity transforms.
         */
        void updateSources();

        /**
         * @brief Removes sounds that have finished playing.
         */
        void cleanupFinishedSounds();

        /**
         * @brief Processes queued play requests from AudioSourceComponents.
         */
        void processPlayRequests();

        /**
         * @brief Processes queued stop requests from AudioSourceComponents.
         */
        void processStopRequests();

    private:
        ma_engine mEngine;   /**< Miniaudio engine instance */
        bool mInitialized;  /**< Whether the audio engine is initialized */
        float mMasterVolume;/**< Global master volume */

        /**
         * @brief Active sounds mapped per entity.
         *
         * Multiple sounds per entity are allowed for overlapping playback.
         */
        std::unordered_map<entt::entity, std::vector<PlayingSound>> mPlayingSounds;
    };

}

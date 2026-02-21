#include "strikepch.h"
#include "AudioSystem.h"
#include "StrikeEngine/Scene/Scene.h"
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/World.h"
#include "StrikeEngine/Scene/Components/AudioSourceComponent.h"
#include "StrikeEngine/Scene/Components/AudioListenerComponent.h"
#include "StrikeEngine/Scene/GraphNode.h"

namespace Strike {

    AudioSystem::AudioSystem()
        : mInitialized(false)
        , mMasterVolume(1.0f)
    {
        initialize();
    }

    AudioSystem::~AudioSystem() {
        shutdown();
    }

    bool AudioSystem::initialize() {
        if (mInitialized) {
            return true;
        }


        if (ma_engine_init(NULL, &mEngine) != MA_SUCCESS) {
            STRIKE_CORE_ERROR("Failed to initialize miniaudio engine");
            return false;
        }

        ma_engine_set_volume(&mEngine, mMasterVolume);
        mInitialized = true;
        
        STRIKE_CORE_INFO("AudioSystem initialized");
        return true;
    }

    void AudioSystem::shutdown() {
        if (!mInitialized) {
            return;
        }

        stopAll();
        ma_engine_uninit(&mEngine);
        mInitialized = false;
        
        STRIKE_CORE_INFO("AudioSystem shut down");
    }

    void AudioSystem::onUpdate(float dt) {
        if (!mInitialized) {
            return;
        }

        Scene* scene = World::get().getScene();
        if (!scene) {
            return;
        }

        updateListener();
        processStopRequests();
        processPlayRequests();
        updateSources();
        cleanupFinishedSounds();
    }

    void AudioSystem::updateListener() {
        Scene* scene = World::get().getScene();
        if (!scene) {
            return;
        }

        auto& registry = scene->getRegistry();
        auto view = registry.view<AudioListenerComponent>();

        for (auto entity : view) {
            Entity e(entity, scene);
            if (!e.isActive()) {
                continue;
            }

            glm::vec3 pos = e.getWorldPosition();
            glm::vec3 forward = e.getForward();
            glm::vec3 up = e.getUp();

            ma_engine_listener_set_position(&mEngine, 0, pos.x, pos.y, pos.z);
            ma_engine_listener_set_direction(&mEngine, 0, forward.x, forward.y, forward.z);
            ma_engine_listener_set_world_up(&mEngine, 0, up.x, up.y, up.z);
            
            break; // Only use first active listener
        }
    }

    void AudioSystem::processPlayRequests() {
        Scene* scene = World::get().getScene();
        if (!scene) {
            return;
        }

        auto& registry = scene->getRegistry();
        auto view = registry.view<AudioSourceComponent>();

        for (auto entity : view) {
            auto& source = view.get<AudioSourceComponent>(entity);
            
            if (!source.mPlayRequested || !source.isActive()) {
                continue;
            }

            source.mPlayRequested = false;

            auto audioAsset = source.getAudio();
            if (!audioAsset || !audioAsset->isReady()) {
                continue;
            }

            Entity e(entity, scene);
            if (!e.isActive()) {
                continue;
            }

            auto& soundsVector = mPlayingSounds[entity];
            auto& playingSound = soundsVector.emplace_back();
            
            playingSound.sound = std::make_unique<ma_sound>();
            playingSound.isSpatial = source.isSpatial();
            playingSound.cachedVolume = source.getVolume();

            ma_uint32 flags = MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC;

            
            if (ma_sound_init_from_file(&mEngine, 
                                       audioAsset->getPath().string().c_str(),
                                       flags, 
                                       nullptr, 
                                       nullptr, 
                                       playingSound.sound.get()) != MA_SUCCESS) {
                STRIKE_CORE_ERROR("Failed to initialize sound from file: {}", audioAsset->getPath().string());
                soundsVector.pop_back();
                continue;
            }

            // Configure sound properties
            ma_sound_set_volume(playingSound.sound.get(), source.getVolume());
            ma_sound_set_looping(playingSound.sound.get(), source.isLoop());

            if (source.isSpatial()) {
                glm::vec3 pos = e.getWorldPosition();
                ma_sound_set_position(playingSound.sound.get(), pos.x, pos.y, pos.z);
                ma_sound_set_min_distance(playingSound.sound.get(), source.getMinDistance());
                ma_sound_set_max_distance(playingSound.sound.get(), source.getMaxDistance());
                ma_sound_set_spatialization_enabled(playingSound.sound.get(), MA_TRUE);
            } else {
                ma_sound_set_spatialization_enabled(playingSound.sound.get(), MA_FALSE);
            }

            // Start playback (will play when async loading completes)
            if (ma_sound_start(playingSound.sound.get()) != MA_SUCCESS) {
                ma_sound_uninit(playingSound.sound.get());
                STRIKE_CORE_ERROR("Failed to start sound");
                soundsVector.pop_back();
                continue;
            }
        }
    }

    void AudioSystem::processStopRequests() {
        Scene* scene = World::get().getScene();
        if (!scene) {
            return;
        }

        auto& registry = scene->getRegistry();
        auto view = registry.view<AudioSourceComponent>();

        for (auto entity : view) {
            auto& source = view.get<AudioSourceComponent>(entity);
            
            if (!source.mStopRequested) {
                continue;
            }

            source.mStopRequested = false;
            stopEntity(entity);
        }
    }

    void AudioSystem::updateSources() {
        Scene* scene = World::get().getScene();
        if (!scene) {
            return;
        }

        auto& registry = scene->getRegistry();
        auto view = registry.view<AudioSourceComponent>();

        for (auto entity : view) {
            Entity e(entity, scene);
            if (!e.isActive()) {
                continue;
            }

            auto& source = view.get<AudioSourceComponent>(entity);
            auto it = mPlayingSounds.find(entity);
            if (it == mPlayingSounds.end()) {
                continue;
            }

            // Update all playing sounds for this entity
            for (auto& playingSound : it->second) {
                // Update volume if changed
                if (playingSound.cachedVolume != source.getVolume()) {
                    ma_sound_set_volume(playingSound.sound.get(), source.getVolume());
                    playingSound.cachedVolume = source.getVolume();
                }

                // Update position for spatial sounds
                if (playingSound.isSpatial) {
                    glm::vec3 pos = e.getWorldPosition();
                    ma_sound_set_position(playingSound.sound.get(), pos.x, pos.y, pos.z);
                }
            }
        }
    }

    void AudioSystem::cleanupFinishedSounds() {
        for (auto it = mPlayingSounds.begin(); it != mPlayingSounds.end();) {
            auto& soundsVector = it->second;
            
            // Remove sounds that have finished playing
            soundsVector.erase(
                std::remove_if(soundsVector.begin(), soundsVector.end(),
                    [](PlayingSound& playingSound) {
                        // Check if sound has finished (non-looping sounds only)
                        if (ma_sound_at_end(playingSound.sound.get())) {
                            ma_sound_uninit(playingSound.sound.get());
                            return true; // Remove this sound
                        }
                        return false; // Keep this sound
                    }),
                soundsVector.end()
            );

            // Remove entity entry if no sounds are playing
            if (soundsVector.empty()) {
                it = mPlayingSounds.erase(it);
            } else {
                ++it;
            }
        }
    }

    void AudioSystem::stopEntity(entt::entity entity) {
        auto it = mPlayingSounds.find(entity);
        if (it == mPlayingSounds.end()) {
            return;
        }

        // Stop and uninit all sounds for this entity
        for (auto& playingSound : it->second) {
            ma_sound_stop(playingSound.sound.get());
            ma_sound_uninit(playingSound.sound.get());
        }

        mPlayingSounds.erase(it);
    }

    void AudioSystem::stopAll() {
        for (auto& [entity, soundsVector] : mPlayingSounds) {
            for (auto& playingSound : soundsVector) {
                ma_sound_stop(playingSound.sound.get());
                ma_sound_uninit(playingSound.sound.get());
            }
        }
        mPlayingSounds.clear();
    }

    void AudioSystem::setMasterVolume(float volume) {
        mMasterVolume = glm::clamp(volume, 0.0f, 1.0f);
        if (mInitialized) {
            ma_engine_set_volume(&mEngine, mMasterVolume);
        }
    }

    float AudioSystem::getMasterVolume() const {
        return mMasterVolume;
    }

} 
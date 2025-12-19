#include "strikepch.h"
#include "AudioSystem.h"
#include "StrikeEngine/Scene/World.h"
#include "StrikeEngine/Scene/Scene.h"
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/Components/AudioSourceComponent.h"
#include "StrikeEngine/Scene/Components/AudioListenerComponent.h"
#include "StrikeEngine/Asset/Types/Audio.h"
#include <glm/glm.hpp>


namespace StrikeEngine {

    AudioSystem::AudioSystem() {}

    AudioSystem::~AudioSystem() {
        shutdown();
    }

    bool AudioSystem::initialize() {
        mDeviceConfig = ma_device_config_init(ma_device_type_playback);
        mDeviceConfig.playback.format = ma_format_f32;
        mDeviceConfig.playback.channels = 2;
        mDeviceConfig.sampleRate = 48000;
        mDeviceConfig.dataCallback = dataCallback;
        mDeviceConfig.pUserData = this;

        if (ma_device_init(NULL, &mDeviceConfig, &mDevice) != MA_SUCCESS) {
            std::cerr << "Failed to initialize audio device" << std::endl;
            return false;
        }

        if (ma_device_start(&mDevice) != MA_SUCCESS) {
            std::cerr << "Failed to start audio device" << std::endl;
            ma_device_uninit(&mDevice);
            return false;
        }

        mInitialized = true;
        return true;
    }

    void AudioSystem::shutdown() {
        if (mInitialized) {
            ma_device_uninit(&mDevice);
            mInitialized = false;
        }
    }

    void AudioSystem::onUpdate(float dt) {
        Scene* scene = World::get().getScene();
        if (!scene) return;

        auto& registry = scene->getRegistry();
        auto view = registry.view<AudioSourceComponent>();

        for (auto entity : view) {
            auto& source = registry.get<AudioSourceComponent>(entity);
            if (source.isAutoplay() && source.hasAudio() && !source.isPlaying() && !source.isPaused()) {
                source.play();
            }
        }
    }

    void AudioSystem::dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
        AudioSystem* system = static_cast<AudioSystem*>(pDevice->pUserData);
        system->processAudio(static_cast<float*>(pOutput), frameCount);
    }

    void AudioSystem::processAudio(float* pOutput, ma_uint32 frameCount) {
        Scene* scene = World::get().getScene();
        if (!scene) {
            memset(pOutput, 0, frameCount * 2 * sizeof(float));
            return;
        }

        auto& registry = scene->getRegistry();

        glm::vec3 listenerPos(0.0f);
        auto listenerView = registry.view<AudioListenerComponent>();
        if (listenerView.begin() != listenerView.end()) {
            auto listenerEntity = *listenerView.begin();
            Entity listener(listenerEntity, scene);
            listenerPos = listener.getPosition();
        }

        memset(pOutput, 0, frameCount * 2 * sizeof(float));

        auto view = registry.view<AudioSourceComponent>();
        for (auto entity : view) {
            Entity ent(entity, scene);
            auto& source = registry.get<AudioSourceComponent>(entity);

            if (!source.isPlaying() || source.isPaused() || !source.hasAudio()) continue;

            auto audio = source.getAudio();
            const auto& audioData = audio->getData();
            uint64_t totalFrames = audio->getFrameCount();
            uint32_t channels = audio->getChannels();

            float volume = source.getVolume();

            if (source.isSpatial()) {
                glm::vec3 sourcePos = ent.getPosition();
                float distance = glm::distance(listenerPos, sourcePos);
                float attenuation = 1.0f / (1.0f + distance * 0.1f);
                volume *= attenuation;
            }

            for (ma_uint32 i = 0; i < frameCount; ++i) {
                if (source.mCurrentFrame >= totalFrames) {
                    if (source.isLoop()) {
                        source.mCurrentFrame = 0;
                    } else {
                        source.stop();
                        break;
                    }
                }

                uint64_t sampleIndex = source.mCurrentFrame * channels;

                for (uint32_t ch = 0; ch < 2; ++ch) {
                    float sample = 0.0f;
                    if (channels == 1) {
                        sample = audioData[sampleIndex];
                    } else if (ch < channels) {
                        sample = audioData[sampleIndex + ch];
                    }
                    pOutput[i * 2 + ch] += sample * volume;
                }

                source.mCurrentFrame++;
            }
        }
    }

} // namespace StrikeEngine
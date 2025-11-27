#pragma once
#include "SystemECS.h"
#include <miniaudio.h>

namespace StrikeEngine {
    class Scene;

    class AudioSystem : public SystemECS {
    public:
        AudioSystem();
        ~AudioSystem();

        void onUpdate(float dt) override;

        bool initialize();
        void shutdown();

    private:
        static void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
        void processAudio(float* pOutput, ma_uint32 frameCount);

    private:
        ma_device mDevice;
        ma_device_config mDeviceConfig;
        bool mInitialized = false;

        friend class World;
    };
}
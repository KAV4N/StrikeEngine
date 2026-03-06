#pragma once

#include "Asset.h"
#include <filesystem>
#include <vector>

namespace Strike {

    /**
     * @brief Audio asset type.
     * @note Loaded via AssetManager by AudioLoader. 
     */
    class Audio : public Asset {
    public:
        /**
         * @brief Construct a new Audio object.
         * @param id Unique identifier for the audio asset in AssetManager.
         * @param path File path to the audio asset.
         * @note This does not load the audio asset; for loading use AssetManager.
         */
        Audio(const std::string& id, const std::filesystem::path& path);
        ~Audio() = default;

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "audio";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return Audio::getStaticTypeName();
        }

        /**
         * @brief Get the duration of the audio in seconds.
         */
        float getDuration() const { return mDuration; }

        /**
         * @brief Get the pre-baked RMS amplitude envelope.
         * Each value represents the RMS amplitude of one hop window.
         */
        const std::vector<float>& getAmplitudeEnvelope() const { return mAmplitudeEnvelope; }

        /**
         * @brief Get the duration in seconds of each envelope hop frame.
         */
        float getEnvelopeFrameDuration() const { return mEnvelopeFrameDuration; }

        /**
         * @brief Sample the amplitude envelope at a given playback time.
         * @param timeSeconds Current playback position in seconds.
         * @return RMS amplitude in [0.0, 1.0] at that time, or 0.0 if unavailable.
         */
        float sampleAmplitudeAt(float timeSeconds) const {
            if (mAmplitudeEnvelope.empty() || mEnvelopeFrameDuration <= 0.0f)
                return 0.0f;
            const size_t idx = static_cast<size_t>(timeSeconds / mEnvelopeFrameDuration);
            const size_t clamped = std::min(idx, mAmplitudeEnvelope.size() - 1);
            return mAmplitudeEnvelope[clamped];
        }

    private:
        friend class AudioLoader;

        float mDuration = 0.0f;                    ///< Duration in seconds
        std::vector<float> mAmplitudeEnvelope;      ///< Pre-baked RMS envelope
        float mEnvelopeFrameDuration = 0.0f;        ///< Seconds per envelope sample
    };

}
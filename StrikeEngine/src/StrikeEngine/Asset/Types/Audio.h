#pragma once

#include "Asset.h"
#include <filesystem>

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

    private:
        friend class AudioLoader;

        float mDuration;  ///< Duration in seconds
    };

}
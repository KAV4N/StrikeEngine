#pragma once

#include "Asset.h"
#include <filesystem>

namespace StrikeEngine {

    class Audio : public Asset {
    public:
        Audio(const std::string& id, const std::filesystem::path& path);
        ~Audio() = default;

        static const std::string& getStaticTypeName();
        const std::string& getTypeName() const override;
        
        float getDuration() const { return mDuration; }

    private:
        friend class AudioLoader;

        float mDuration;  // seconds
    };

}
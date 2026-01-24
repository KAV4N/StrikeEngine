#include "strikepch.h"
#include "Audio.h"

namespace StrikeEngine {

    Audio::Audio(const std::string& id, const std::filesystem::path& path)
        : Asset(id, path)
        , mDuration(0.0f)
    {
    }

    const std::string& Audio::getStaticTypeName() {
        static std::string typeName = "audio";
        return typeName;
    }

    const std::string& Audio::getTypeName() const {
        return getStaticTypeName();
    }
}
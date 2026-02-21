#include "strikepch.h"
#include "Audio.h"

namespace Strike {

    Audio::Audio(const std::string& id, const std::filesystem::path& path)
        : Asset(id, path)
        , mDuration(0.0f)
    {
    }

}
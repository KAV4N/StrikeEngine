#pragma once

#include "StrikeEngine/Asset/Types/Asset.h"

#include <filesystem>
#include <memory>
#include <string>

namespace StrikeEngine {

    class AssetLoader {
    public:
        AssetLoader(AssetType type);
        virtual ~AssetLoader() = default;
        virtual std::shared_ptr<Asset> load(const std::string& id, const std::filesystem::path& path) = 0;
        AssetType getType() const;

    protected:
        AssetType mAssetType;
    };

}
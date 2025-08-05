#include "AssetLoader.h"

namespace StrikeEngine {

    AssetLoader::AssetLoader(AssetType type) : mAssetType(type) {}

    AssetType AssetLoader::getType() const {
        return mAssetType;
    }

}
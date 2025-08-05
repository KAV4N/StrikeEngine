#include "Asset.h"
#include "StrikeEngine/Asset/AssetManager.h"

namespace StrikeEngine {
    Asset::Asset(AssetType type, const std::string& id, const std::filesystem::path& path, const std::string& name)
        : mId(id), mType(type), mPath(path), mName(name), mLoadingState(AssetLoadingState::Uninitialized) {
    }

    Asset::~Asset() {
        if (!mId.empty())
            AssetManager::get().removeAssetInternal(mId);
    }

    const std::string& Asset::getId() const {
        return mId;
    }

    AssetType Asset::getType() const {
        return mType;
    }

    AssetLoadingState Asset::getLoadingState() const {
        return mLoadingState;
    }

    const std::filesystem::path& Asset::getPath() const {
        return mPath;
    }

    const std::string& Asset::getName() const {
        return mName;
    }

    void Asset::setName(const std::string& name) {
        mName = name;
    }

    bool Asset::isReady() const {
        return mLoadingState == AssetLoadingState::Ready;
    }

    bool Asset::isLoading() const {
        return mLoadingState == AssetLoadingState::Loading;
    }

    bool Asset::hasFailed() const {
        return mLoadingState == AssetLoadingState::FAILED;
    }

    void Asset::setLoadingState(AssetLoadingState state) {
        mLoadingState = state;
    }
}
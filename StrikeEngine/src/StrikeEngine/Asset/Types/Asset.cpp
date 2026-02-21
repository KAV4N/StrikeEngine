#include "strikepch.h"
#include "Asset.h"
#include "StrikeEngine/Asset/AssetManager.h"


namespace Strike {
    Asset::Asset(const std::string& id, const std::filesystem::path& path)
        : mId(id),
        mPath(path),
        mLoadingState(AssetState::Uninitialized),
        mNeedsPostLoad(false)
    {
    }

    Asset::~Asset() {
    }

    void Asset::postLoad() {
    }

    const std::string& Asset::getId() const {
        return mId;
    }

    AssetState Asset::getState() const {
        return mLoadingState;
    }

    const std::filesystem::path& Asset::getPath() const {
        return mPath;
    }



    bool Asset::isReady() const {
        return mLoadingState == AssetState::Ready;
    }

    bool Asset::isLoading() const {
        return mLoadingState == AssetState::Loading;
    }


    bool Asset::hasFailed() const {
        return mLoadingState == AssetState::Failed;
    }

    void Asset::setState(AssetState state) {
        mLoadingState = state;
    }

    std::filesystem::path Asset::addRootPrefix(const std::filesystem::path& path) {
        std::string pathStr = path.generic_string();

        if (!pathStr.empty() && pathStr[0] == '@') {
            return std::filesystem::path(pathStr).lexically_normal();
        }
        return std::filesystem::path("@") / path.lexically_normal();
    }
}
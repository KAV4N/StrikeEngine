#include "Asset.h"
#include "StrikeEngine/Asset/AssetManager.h"

namespace StrikeEngine {
    Asset::Asset(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : mId(id),
        mPath(path),
        mName(name),
        mLoadingState(AssetLoadingState::Uninitialized)
    {
    }

    Asset::~Asset() {
        /*
        std::cout << "im dying" << mId << std::endl;
        if (mIsPlaceHolder)
            AssetManager::get().removeAssetInternal(mId);
        */
    }

    const std::string& Asset::getId() const {
        return mId;
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

        // Call postLoad when asset becomes ready
        if (state == AssetLoadingState::Ready) {
            postLoad();
        }
    }

    pugi::xml_node Asset::toNode() const {
        pugi::xml_document doc;
        pugi::xml_node node = doc.append_child(getTypeName());
        node.append_attribute("assetId") = getId().c_str();
        node.append_attribute("src") = mPath.string().c_str();
        return node;
    }
}
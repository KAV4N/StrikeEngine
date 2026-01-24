#include "strikepch.h"
#include "Asset.h"
#include "StrikeEngine/Asset/AssetManager.h"


namespace StrikeEngine {
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

    AssetState Asset::getLoadingState() const {
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

    void Asset::setLoadingState(AssetState state) {
        mLoadingState = state;
    }

    void Asset::toNode(pugi::xml_node parent) {
        pugi::xml_node node = parent.append_child(getTypeName().c_str());
        node.append_attribute("id") = mId.c_str();
        node.append_attribute("src") = addRootPrefix(mPath).generic_string().c_str();
    }

    std::filesystem::path Asset::addRootPrefix(const std::filesystem::path& path) {
        std::string pathStr = path.generic_string();

        if (!pathStr.empty() && pathStr[0] == '@') {
            return std::filesystem::path(pathStr).lexically_normal();
        }
        return std::filesystem::path("@") / path.lexically_normal();
    }
}
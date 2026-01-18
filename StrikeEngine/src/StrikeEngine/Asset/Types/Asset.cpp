#include "strikepch.h"
#include "Asset.h"
#include "StrikeEngine/Asset/AssetManager.h"


namespace StrikeEngine {
    Asset::Asset(const std::string& id, const std::filesystem::path& path)
        : mId(id),
        mPath(path),
        mLoadingState(AssetState::Uninitialized),
        mLoadAsync(false),
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

    bool Asset::isAsync() const {
        return mLoadAsync;
    }

    void Asset::setLoadAsync(bool async) {
        mLoadAsync = async;
    }

    void Asset::setLoadingState(AssetState state) {
        mLoadingState = state;
    }

    void Asset::toNode(pugi::xml_node parent) const {
        pugi::xml_node node = parent.append_child(getTypeName().c_str());
        node.append_attribute("id") = mId.c_str();
        node.append_attribute("src") = mPath.string().c_str();
    }
}
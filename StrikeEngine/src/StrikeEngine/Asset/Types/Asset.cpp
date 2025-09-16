#include "Asset.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include <mutex>

namespace StrikeEngine {
    Asset::Asset(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : mId(id),
        mPath(path),
        mName(name),
        mLoadingState(AssetLoadingState::Uninitialized),
        mLoadAsync(false)
    {
    }

    Asset::~Asset() {
    }

    void Asset::postLoad() {
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

    bool Asset::isLoaded() const {
        return mLoadingState == AssetLoadingState::Loaded;
    }

    bool Asset::hasFailed() const {
        return mLoadingState == AssetLoadingState::FAILED;
    }

    bool Asset::isAsync() const {
        return mLoadAsync;
    }

    void Asset::setLoadAsync(bool async) {
        mLoadAsync = async;
    }

    void Asset::setLoadingState(AssetLoadingState state) {
        mLoadingState = state;
    }

    pugi::xml_node Asset::toNode() const {
        pugi::xml_document doc;
        pugi::xml_node node = doc.append_child(getTypeName().c_str());
        node.append_attribute("assetId") = mId.c_str();
        node.append_attribute("name") = mName.c_str();
        node.append_attribute("src") = mPath.string().c_str();
        return node;
    }
}
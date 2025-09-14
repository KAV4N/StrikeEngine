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
        std::lock_guard<std::mutex> lock(mMutex);
        return mId;
    }

    AssetLoadingState Asset::getLoadingState() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mLoadingState;
    }

    const std::filesystem::path& Asset::getPath() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mPath;
    }

    const std::string& Asset::getName() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mName;
    }

    void Asset::setName(const std::string& name) {
        std::lock_guard<std::mutex> lock(mMutex);
        mName = name;
    }

    bool Asset::isReady() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mLoadingState == AssetLoadingState::Ready;
    }

    bool Asset::isLoading() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mLoadingState == AssetLoadingState::Loading;
    }

    bool Asset::isLoaded() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mLoadingState == AssetLoadingState::Loaded;
    }

    bool Asset::hasFailed() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mLoadingState == AssetLoadingState::FAILED;
    }

    bool Asset::isAsync() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mLoadAsync;
    }

    void Asset::setLoadAsync(bool async) {
        std::lock_guard<std::mutex> lock(mMutex);
        mLoadAsync = async;
    }

    void Asset::setLoadingState(AssetLoadingState state) {
        std::lock_guard<std::mutex> lock(mMutex);
        mLoadingState = state;
    }

    pugi::xml_node Asset::toNode() const {
        std::lock_guard<std::mutex> lock(mMutex);
        pugi::xml_document doc;
        pugi::xml_node node = doc.append_child(getTypeName().c_str());
        node.append_attribute("assetId") = mId.c_str();
        node.append_attribute("name") = mName.c_str();
        node.append_attribute("src") = mPath.string().c_str();
        return node;
    }
}
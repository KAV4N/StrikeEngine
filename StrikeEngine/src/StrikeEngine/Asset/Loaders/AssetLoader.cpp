#include "strikepch.h"
#include "AssetLoader.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "StrikeEngine/Asset/Types/Model.h"


namespace StrikeEngine {

    AssetLoader::AssetLoader(const std::string& typeName) : mTypeName(typeName) {}

    const std::string& AssetLoader::getTypeName() const {
        return mTypeName;
    }

    std::shared_ptr<Asset> AssetLoader::load(const std::string& id, const std::filesystem::path& path) {
        {
            std::lock_guard<std::mutex> lock(mMutex);

            auto it = mLoadingTasks.find(id);
            if (it != mLoadingTasks.end()) {
                return it->second.placeholderAsset;
            }
        }

        auto asset = loadAssetInternal(id, path);

        if (!asset->mNeedsPostLoad){
            asset->postLoad();
            asset->setLoadingState(AssetState::Ready);
        }else{
            LoadingTask postLoadTask;
            postLoadTask.placeholderAsset = asset;
            mLoadingTasks.emplace(asset->getId(), std::move(postLoadTask));
        }

        return asset;
    }

    void AssetLoader::loadAsync(const std::string& id, const std::filesystem::path& path, std::shared_ptr<Asset> placeholderAsset) {
        std::lock_guard<std::mutex> lock(mMutex);
        placeholderAsset->setLoadingState(AssetState::Loading);

        if (mLoadingTasks.find(id) != mLoadingTasks.end()) {
            return;
        }
    
        placeholderAsset->setLoadAsync(true);
        auto future = std::async(std::launch::async, [this, id, path]() {
            return this->loadAssetInternal(id, path, true);
        });
        mLoadingTasks.emplace(id, LoadingTask(id, path, placeholderAsset, std::move(future)));
    }

    void AssetLoader::update() {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadingTasks.begin();
        while (it != mLoadingTasks.end()) {
            auto& task = it->second;
            auto userAsset = task.placeholderAsset;
            if (userAsset->mNeedsPostLoad && !task.future.valid()){
                userAsset->postLoad();
                userAsset->setLoadingState(AssetState::Ready);
                it = mLoadingTasks.erase(it);
            }
            else if (task.future.valid() && task.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                auto loadedAsset = task.future.get();
                
                if (loadedAsset) {
                    swapData(userAsset, loadedAsset);
                    userAsset->postLoad();
                    userAsset->mLoadAsync = true;
                    userAsset->setLoadingState(AssetState::Ready);
                }
                else {
                    userAsset->setLoadingState(AssetState::Failed);
                    //AssetManager::get().removeAsset(userAsset->getId()); 
                }
                
                it = mLoadingTasks.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void AssetLoader::clearLoadingTasks() {
        std::lock_guard<std::mutex> lock(mMutex);
        for (auto& [id, task] : mLoadingTasks) {
            if (task.future.valid()) {
                auto status = task.future.wait_for(std::chrono::milliseconds(500));
                
                if (status == std::future_status::ready) {
                    try {
                        task.future.get(); 
                    } catch (const std::exception& e) {
                        STRIKE_CORE_ERROR("Error during async task cleanup: {}", e.what());
                    } catch (...) {
                    }
                }
            }
            
            if (task.placeholderAsset) {
                task.placeholderAsset->setLoadingState(AssetState::Failed);
            }
        }
        
        mLoadingTasks.clear();
    }

    bool AssetLoader::hasLoadingTasks() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return !mLoadingTasks.empty();
    }

    std::filesystem::path AssetLoader::resolvePath(const std::filesystem::path& src, const std::filesystem::path& basePath) const {
        std::string srcString = src.string();

        if (!srcString.empty() && srcString[0] == '@') {
            std::string pathWithoutAt = srcString.substr(1);

            if (!pathWithoutAt.empty() && (pathWithoutAt[0] == '/' || pathWithoutAt[0] == '\\')) {
                pathWithoutAt = pathWithoutAt.substr(1);
            }
            return std::filesystem::path(pathWithoutAt).lexically_normal();
        }
        else {
            return (basePath / src).lexically_normal();
        }
    }

    std::filesystem::path AssetLoader::addRootPrefix(const std::filesystem::path& path) {
        std::string pathStr = path.generic_string();

        if (!pathStr.empty() && pathStr[0] == '@') {
            return std::filesystem::path(pathStr).lexically_normal();
        }
        return std::filesystem::path("@") / path.lexically_normal();
    }

}
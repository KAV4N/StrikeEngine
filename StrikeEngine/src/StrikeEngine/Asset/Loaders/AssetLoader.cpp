#include "AssetLoader.h"
#include <algorithm>
#include "StrikeEngine/Asset/AssetManager.h"

namespace StrikeEngine {

    AssetLoader::AssetLoader(const std::string& typeName) : mTypeName(typeName) {}

    const std::string& AssetLoader::getTypeName() const {
        return mTypeName;
    }

    std::shared_ptr<Asset> AssetLoader::load(const std::string& id, const std::filesystem::path& path, bool async) {
        return nullptr;
    }

    void AssetLoader::loadAsync(const std::string& id, const std::filesystem::path& path, std::shared_ptr<Asset> placeholderAsset) {
        std::lock_guard<std::mutex> lock(mMutex);

        if (mLoadingTasks.find(id) != mLoadingTasks.end()) {
            return; 
        }

        auto future = std::async(std::launch::async, [this, id, path]() {
            return this->load(id, path, true);
            });

        mLoadingTasks.emplace(id, LoadingTask(id, path, placeholderAsset, std::move(future)));

    }

    void AssetLoader::update() {
        std::lock_guard<std::mutex> lock(mMutex);

        auto it = mLoadingTasks.begin();
        while (it != mLoadingTasks.end()) {
            auto& task = it->second;
            if (task.flagOnlyPostLoad && task.placeholderAsset->isLoaded()) {
                task.placeholderAsset->postLoad();
                task.placeholderAsset->setLoadingState(AssetLoadingState::Ready);
                it = mLoadingTasks.erase(it);
                //auto& assetM = AssetManager::get();
                
            }
            else if (task.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                try {
                    auto loadedAsset = task.future.get();

                    if (loadedAsset && task.placeholderAsset) {
                        swapData(task.placeholderAsset, loadedAsset);
                        task.placeholderAsset->postLoad();
                        task.placeholderAsset->setLoadingState(AssetLoadingState::Ready);

                    }
                    else if (task.placeholderAsset) {
                        // Loading failed
                        task.placeholderAsset->setLoadingState(AssetLoadingState::FAILED);
                    }
                }
                catch (const std::exception& e) {
                    if (task.placeholderAsset) {
                        task.placeholderAsset->setLoadingState(AssetLoadingState::FAILED);
                    }
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

        // Set all pending assets to failed state
        for (auto& [id, task] : mLoadingTasks) {
            if (task.placeholderAsset) {
                task.placeholderAsset->setLoadingState(AssetLoadingState::FAILED);
            }
        }

        mLoadingTasks.clear();
    }

}
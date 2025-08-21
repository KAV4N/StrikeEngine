#include "AssetLoader.h"
#include <algorithm>

namespace StrikeEngine {

    AssetLoader::AssetLoader(const std::string& typeName) : mTypeName(typeName) {}

    const std::string& AssetLoader::getTypeName() const {
        return mTypeName;
    }

    void AssetLoader::loadAsync(const std::string& id, const std::filesystem::path& path, std::shared_ptr<Asset> placeholderAsset) {
        std::lock_guard<std::mutex> lock(mMutex);

        // Check if this asset is already being loaded
        if (mLoadingTasks.find(id) != mLoadingTasks.end()) {
            return; // Already loading
        }

        // Start async loading
        auto future = std::async(std::launch::async, [this, id, path]() {
            return this->load(id, path);
            });

        // Store the loading task
        mLoadingTasks.emplace(id, LoadingTask(id, path, placeholderAsset, std::move(future)));
    }

    bool AssetLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loadedAsset) {
        return placeholder->swapData(*loadedAsset);
    }

    void AssetLoader::update() {
        std::lock_guard<std::mutex> lock(mMutex);

        auto it = mLoadingTasks.begin();
        while (it != mLoadingTasks.end()) {
            auto& task = it->second;

            if (task.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                try {
                    auto loadedAsset = task.future.get();

                    if (loadedAsset && task.placeholderAsset) {
                        if (swapData(task.placeholderAsset, loadedAsset)) {
                            task.placeholderAsset->setLoadingState(AssetLoadingState::Ready);
                        }
                        else {
                            task.placeholderAsset->setLoadingState(AssetLoadingState::FAILED);
                        }
                    }
                    else if (task.placeholderAsset){
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
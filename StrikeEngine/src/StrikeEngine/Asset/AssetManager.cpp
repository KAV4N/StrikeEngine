#include "AssetManager.h"

#include <iostream>
#include <algorithm>
#include "StrikeEngine/Asset/Types/Mesh.h"
#include "StrikeEngine/Asset/Types/Material.h"
#include "StrikeEngine/Asset/Loaders/MeshLoader.h"

namespace StrikeEngine {

    AssetManager::AssetManager() {
        registerAssetLoaders();
    }

    void AssetManager::registerAssetLoaders() {
        registerLoader<Mesh>(std::make_unique<MeshLoader>());
    }

    AssetManager::~AssetManager() {
        clear();
    }

    void AssetManager::registerLoaderByType(AssetType type, std::unique_ptr<AssetLoader> loader) {
        std::lock_guard<std::mutex> lock(mMutex);
        mLoaders[type] = std::move(loader);
    }

    std::shared_ptr<Asset> AssetManager::getAssetBase(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            auto asset = it->second.lock();
            if (asset) {
                return asset;
            }
            else {
                mLoadedAssets.erase(it);
            }
        }
        return nullptr;
    }

    bool AssetManager::hasAsset(const std::string& id) const {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        return it != mLoadedAssets.end() && !it->second.expired();
    }

    bool AssetManager::isAssetLoading(const std::string& id) const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mLoadingTasks.find(id) != mLoadingTasks.end();
    }

    void AssetManager::removeAssetInternal(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        mLoadedAssets.erase(id);
        mLoadingTasks.erase(id);
    }

    void AssetManager::clear() {
        std::lock_guard<std::mutex> lock(mMutex);
        mLoadedAssets.clear();
        mLoadingTasks.clear();
    }

    void AssetManager::update() {
        std::lock_guard<std::mutex> lock(mMutex);

        for (auto it = mLoadingTasks.begin(); it != mLoadingTasks.end(); ) {
            if (it->second.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                try {
                    auto loadedAsset = it->second.future.get();
                    if (loadedAsset) {
                        auto& placeholder = it->second.placeholderAsset;
                        if (placeholder->swapData(*loadedAsset)) {
                            placeholder->setLoadingState(AssetLoadingState::Ready);
                        }
                        else {
                            std::cerr << "Failed to swap asset data for '" << it->first
                                << "': Type mismatch or swap failed" << std::endl;
                            placeholder->setLoadingState(AssetLoadingState::FAILED);
                        }
                    }
                    else {
                        it->second.placeholderAsset->setLoadingState(AssetLoadingState::FAILED);
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "Error loading asset '" << it->first << "': " << e.what() << std::endl;
                    it->second.placeholderAsset->setLoadingState(AssetLoadingState::FAILED);
                }

                it = mLoadingTasks.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    std::vector<std::string> AssetManager::getLoadedAssetIds() const {
        std::lock_guard<std::mutex> lock(mMutex);
        std::vector<std::string> loadedIds;
        for (const auto& [id, weakAsset] : mLoadedAssets) {
            if (!weakAsset.expired()) {
                auto asset = weakAsset.lock();
                if (asset && asset->getLoadingState() == AssetLoadingState::Ready) {
                    loadedIds.push_back(id);
                }
            }
        }
        return loadedIds;
    }

    std::vector<std::string> AssetManager::getLoadingAssetIds() const {
        std::lock_guard<std::mutex> lock(mMutex);
        std::vector<std::string> loadingIds;
        for (const auto& [id, task] : mLoadingTasks) {
            loadingIds.push_back(id);
        }
        return loadingIds;
    }

    size_t AssetManager::getLoadedAssetCount() const {
        std::lock_guard<std::mutex> lock(mMutex);
        size_t count = 0;
        for (const auto& [id, weakAsset] : mLoadedAssets) {
            if (!weakAsset.expired()) {
                auto asset = weakAsset.lock();
                if (asset && asset->getLoadingState() == AssetLoadingState::Ready) {
                    ++count;
                }
            }
        }
        return count;
    }

    size_t AssetManager::getLoadingAssetCount() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mLoadingTasks.size();
    }

}
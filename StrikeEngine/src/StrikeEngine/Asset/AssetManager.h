#pragma once

#include "StrikeEngine/Asset/Types/Asset.h"
#include "StrikeEngine/Asset/Loaders/AssetLoader.h"
#include <filesystem>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace StrikeEngine {
    class AssetLoader;

    class AssetManager {
    public:
        static AssetManager& get() {
            static AssetManager instance;
            return instance;
        }

        template <typename AssetT>
        void registerLoader(std::unique_ptr<AssetLoader> loader) {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            AssetType type = AssetT::getStaticAssetType();
            registerLoaderByType(type, std::move(loader));
        }

        void registerLoaderByType(AssetType type, std::unique_ptr<AssetLoader> loader);

        template <typename AssetT>
        std::shared_ptr<AssetT> loadAsset(const std::string& id, const std::filesystem::path& filePath) {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            if (id.empty()) {
                std::cerr << "Error: Asset ID cannot be empty" << std::endl;
                return nullptr;
            }
            std::lock_guard<std::mutex> lock(mMutex);

            auto loadedIt = mLoadedAssets.find(id);
            if (loadedIt != mLoadedAssets.end()) {
                auto existingAsset = loadedIt->second.lock();
                if (existingAsset) {
                    return std::dynamic_pointer_cast<AssetT>(existingAsset);
                }
                else {
                    mLoadedAssets.erase(loadedIt);
                }
            }

            auto loadingIt = mLoadingTasks.find(id);
            if (loadingIt != mLoadingTasks.end()) {
                return std::dynamic_pointer_cast<AssetT>(loadingIt->second.placeholderAsset);
            }

            AssetType type = AssetT::getStaticAssetType();
            auto loaderIt = mLoaders.find(type);
            if (loaderIt == mLoaders.end()) {
                std::cerr << "No loader registered for asset type: " << static_cast<int>(type) << std::endl;
                return nullptr;
            }

            auto asset = loaderIt->second->load(id, filePath);
            if (asset) {
                asset->setLoadingState(AssetLoadingState::Ready);
                mLoadedAssets[id] = asset;
            }

            return std::dynamic_pointer_cast<AssetT>(asset);
        }

        template <typename AssetT>
        std::shared_ptr<AssetT> loadAssetAsync(const std::string& id, const std::filesystem::path& filePath) {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            if (id.empty()) {
                std::cerr << "Error: Asset ID cannot be empty" << std::endl;
                return nullptr;
            }
            std::lock_guard<std::mutex> lock(mMutex);

            auto loadedIt = mLoadedAssets.find(id);
            if (loadedIt != mLoadedAssets.end()) {
                auto existingAsset = loadedIt->second.lock();
                if (existingAsset) {
                    return std::dynamic_pointer_cast<AssetT>(existingAsset);
                }
                else {
                    mLoadedAssets.erase(loadedIt);
                }
            }

            auto loadingIt = mLoadingTasks.find(id);
            if (loadingIt != mLoadingTasks.end()) {
                return std::dynamic_pointer_cast<AssetT>(loadingIt->second.placeholderAsset);
            }

            AssetType type = AssetT::getStaticAssetType();
            auto loaderIt = mLoaders.find(type);
            if (loaderIt == mLoaders.end()) {
                std::cerr << "No loader registered for asset type: " << static_cast<int>(type) << std::endl;
                return nullptr;
            }

            std::shared_ptr<Asset> placeholderAsset;
            placeholderAsset = std::make_shared<AssetT>(id, filePath, "Untitled");
            placeholderAsset->setLoadingState(AssetLoadingState::Loading);

            mLoadedAssets[id] = placeholderAsset;

            auto loader = loaderIt->second.get();
            std::string emptyId = "";
            auto future = std::async(std::launch::async, [loader, emptyId, filePath]() {
                return loader->load(emptyId, filePath);
                });

            LoadingTask task = { id, type, placeholderAsset, std::move(future) };
            mLoadingTasks[id] = std::move(task);

            return std::dynamic_pointer_cast<AssetT>(placeholderAsset);
        }

        template <typename T>
        std::shared_ptr<T> getAsset(const std::string& id) {
            static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");
            if (id.empty()) {
                std::cerr << "Error: Asset ID cannot be empty" << std::endl;
                return nullptr;
            }
            auto baseAsset = getAssetBase(id);
            return std::dynamic_pointer_cast<T>(baseAsset);
        }

        bool hasAsset(const std::string& id) const;
        bool isAssetLoading(const std::string& id) const;
        void clear();
        void update();

        std::vector<std::string> getLoadedAssetIds() const;
        std::vector<std::string> getLoadingAssetIds() const;
        size_t getLoadedAssetCount() const;
        size_t getLoadingAssetCount() const;

    private:
        AssetManager();
        ~AssetManager();
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        void registerAssetLoaders();
        std::shared_ptr<Asset> getAssetBase(const std::string& id);
        void removeAssetInternal(const std::string& id);

    private:
        friend class Asset;

        struct LoadingTask {
            std::string id;
            AssetType type;
            std::shared_ptr<Asset> placeholderAsset;
            std::future<std::shared_ptr<Asset>> future;
        };

        std::unordered_map<std::string, std::weak_ptr<Asset>> mLoadedAssets;
        std::unordered_map<std::string, LoadingTask> mLoadingTasks;
        std::unordered_map<AssetType, std::unique_ptr<AssetLoader>> mLoaders;
        mutable std::mutex mMutex;
    };
}
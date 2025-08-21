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
#include <thread>
#include <pugixml.hpp>

namespace StrikeEngine {

    class AssetLoader;

    class AssetManager {
    public:
        static AssetManager& get();

        template <typename AssetT>
        std::shared_ptr<AssetT> loadAsset(const std::string& id, const std::filesystem::path& filePath) {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");

            if (auto existingAsset = getAsset<AssetT>(id)) {
                return existingAsset;
            }

            const std::string typeName = AssetT::getStaticTypeName();
            auto loader = getLoader(typeName);
            if (!loader) {
                return nullptr;
            }


            return loadedAsset = loader->load(id, filePath);
        }

        template <typename AssetT>
        std::shared_ptr<AssetT> loadAssetAsync(const std::string& id, const std::filesystem::path& filePath) {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");

            if (auto existingAsset = getAsset<AssetT>(id)) {
                return existingAsset;
            }

            const std::string typeName = AssetT::getStaticTypeName();
            auto loader = getLoader(typeName);
            if (!loader) {
                return nullptr;
            }

            auto placeholderAsset = std::make_shared<AssetT>(id, filePath, filePath.stem().string());
            placeholderAsset->setLoadingState(AssetLoadingState::Loading);

            {
                std::lock_guard<std::mutex> lock(mMutex);
                mLoadedAssets[id] = placeholderAsset;
            }

            loader->loadAsync(id, filePath, placeholderAsset);

            return placeholderAsset;
        }

        template <typename T>
        std::shared_ptr<T> getAsset(const std::string& id) {
            static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

            std::lock_guard<std::mutex> lock(mMutex);
            auto it = mLoadedAssets.find(id);
            if (it != mLoadedAssets.end()) {
                if (auto asset = it->second.lock()) {
                    return std::static_pointer_cast<T>(asset);
                }
                mLoadedAssets.erase(it);
            }
            return nullptr;
        }

        std::shared_ptr<Asset> getAsset(const std::string& type, const std::string& id);

        std::shared_ptr<Asset> loadAsset(const std::string& type, const std::string& id, const std::filesystem::path& filePath);
        std::shared_ptr<Asset> loadAssetAsync(const std::string& type, const std::string& id, const std::filesystem::path& filePath);

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
        AssetLoader* getLoader(const std::string& typeName);

        friend class Asset;

        std::unordered_map<std::string, std::weak_ptr<Asset>> mLoadedAssets;
        std::unordered_map<std::string, std::unique_ptr<AssetLoader>> mLoaders;
        mutable std::mutex mMutex;
    };

} 


#pragma once
#include "StrikeEngine/Core/Log.h"

#include "StrikeEngine/Asset/Types/Asset.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Template.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Asset/Types/Audio.h"

#include <filesystem>
#include <future>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <shared_mutex>
#include <pugixml.hpp>

namespace Strike {
    class AssetLoader;
    
    class AssetManager {
    public:
        static AssetManager& get();

        template<typename T>
        std::shared_ptr<T> load(const std::string& id, const std::filesystem::path& src);
        
        template<typename T>
        std::shared_ptr<T> loadAsync(const std::string& id, const std::filesystem::path& src);
        
        template<typename T>
        std::shared_ptr<T> getAsset(const std::string& id);

        std::shared_ptr<Asset> getAssetBase(const std::string& id) const;
        bool hasAsset(const std::string& id) const;
        bool isAssetLoading(const std::string& id) const;
        bool isShuttingDown() const;
        bool isLoading() const;

        void clear();
        void shutdown();
        void update();
        void removeAsset(const std::string& id);

        std::vector<std::string> getLoadedAssetIds() const;
        std::vector<std::string> getLoadingAssetIds() const;
        size_t getLoadedAssetCount() const;
        size_t getLoadingAssetCount() const;

        void deserialize(const pugi::xml_node& node, const std::filesystem::path& basePath, bool direct = false);

    private:
        AssetManager();
        ~AssetManager();

        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        void registerAssetLoaders();
        void removeAssetInternal(const std::string& id);
        AssetLoader* getLoader(const std::string& typeName);
        std::shared_ptr<Asset> loadInternal(const std::string& id, std::filesystem::path filePath, const std::string& assetType, std::shared_ptr<Asset> placeholder, bool async);

        friend class Asset;
        friend class SceneLoader;
        friend class TemplateLoader;

        mutable std::shared_mutex mAssetsMutex; 

        std::unordered_map<std::string, std::shared_ptr<Asset>> mLoadedAssets;
        std::unordered_map<std::string, std::unique_ptr<AssetLoader>> mLoaders;
        std::atomic<bool> mShuttingDown{false};
    };

    template<typename T>
    std::shared_ptr<T> AssetManager::load(const std::string& id, const std::filesystem::path& filePath) {
        static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset");

        auto placeholder = std::make_shared<T>(id, filePath);

        if (mShuttingDown) {
            placeholder->setState(AssetState::Failed);
            return placeholder;
        }

        if constexpr (std::is_same_v<T, Template>) {
            STRIKE_CORE_ASSERT(filePath.extension() != ".tmpl",
                "Asset '{}': use the model path (e.g. .obj) not .tmpl directly", id);
        }

        {
            std::shared_lock lock(mAssetsMutex);  // read lock for lookup
            auto it = mLoadedAssets.find(id);
            if (it != mLoadedAssets.end()) {
                STRIKE_CORE_ASSERT(it->second->getTypeName() == T::getStaticTypeName(),
                    "Asset '{}': already exists with different type. Expected '{}', got '{}'",
                    id, T::getStaticTypeName(), it->second->getTypeName());
                return std::static_pointer_cast<T>(it->second);
            }
        }

        return std::static_pointer_cast<T>(loadInternal(id, filePath, T::getStaticTypeName(), placeholder, false));
    }

    template<typename T>
    std::shared_ptr<T> AssetManager::loadAsync(const std::string& id, const std::filesystem::path& filePath) {
        static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset");

        auto placeholder = std::make_shared<T>(id, filePath);

        if (mShuttingDown) {
            placeholder->setState(AssetState::Failed);
            return placeholder;
        }

        if constexpr (std::is_same_v<T, Template>) {
            STRIKE_CORE_ASSERT(filePath.extension() != ".tmpl",
                "Asset '{}': use the model path (e.g. .obj) not .tmpl directly", id);
        }

        {
            std::shared_lock lock(mAssetsMutex);  // read lock for lookup
            auto it = mLoadedAssets.find(id);
            if (it != mLoadedAssets.end()) {
                STRIKE_CORE_ASSERT(it->second->getTypeName() == T::getStaticTypeName(),
                    "Asset '{}': already exists with different type. Expected '{}', got '{}'",
                    id, T::getStaticTypeName(), it->second->getTypeName());
                return std::static_pointer_cast<T>(it->second);
            }
        }

        return std::static_pointer_cast<T>(loadInternal(id, filePath, T::getStaticTypeName(), placeholder, true));
    }

    template<typename T>
    std::shared_ptr<T> AssetManager::getAsset(const std::string& id) {
        static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

        std::shared_lock lock(mAssetsMutex);  // read lock
        auto it = mLoadedAssets.find(id);
        if (it == mLoadedAssets.end()) {
            return nullptr;
        }

        STRIKE_CORE_ASSERT(it->second->getTypeName() == T::getStaticTypeName(),
            "Type mismatch for asset '{}': expected '{}', got '{}'",
            id, T::getStaticTypeName(), it->second->getTypeName());

        return std::static_pointer_cast<T>(it->second);
    }
}
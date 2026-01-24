#pragma once

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
#include <pugixml.hpp>

namespace StrikeEngine {
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

        friend class Asset;
        friend class SceneLoader;
        friend class TemplateLoader;

        std::unordered_map<std::string, std::shared_ptr<Asset>> mLoadedAssets;
        std::unordered_map<std::string, std::unique_ptr<AssetLoader>> mLoaders;
        std::atomic<bool> mShuttingDown{false};
    };

    // Template implementations
    template<typename T>
    std::shared_ptr<T> AssetManager::load(const std::string& id, const std::filesystem::path& filePath) {
        static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset");
        
        if (mShuttingDown) {
            return nullptr;
        }
        
        // Check if asset already exists with this ID
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            // Asset exists - check if type matches
            if (it->second->getTypeName() != T::getStaticTypeName()) {
                STRIKE_CORE_ERROR(
                    "Asset with id '{}' already exists with different type. Expected '{}', got '{}'",
                    id,
                    T::getStaticTypeName(),
                    it->second->getTypeName()
                );
                return nullptr;
            }
            
            // Type matches, return existing asset
            return std::static_pointer_cast<T>(it->second);
        }

        auto loader = getLoader(T::getStaticTypeName());
        if (!loader) {
            STRIKE_CORE_ERROR("No loader found for asset type '{}'", T::getStaticTypeName());
            return nullptr;
        }

        auto loadedAsset = loader->load(id, filePath);
        if (loadedAsset) {
            mLoadedAssets[id] = loadedAsset;
        }
        return std::static_pointer_cast<T>(loadedAsset);
    }

    template<typename T>
    std::shared_ptr<T> AssetManager::loadAsync(const std::string& id, const std::filesystem::path& filePath) {
        static_assert(std::is_base_of<Asset, T>::value, "T must derive from Asset");
        
        if (mShuttingDown) {
            return nullptr;
        }
        
        // Check if asset already exists with this ID
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            // Asset exists - check if type matches
            if (it->second->getTypeName() != T::getStaticTypeName()) {
                STRIKE_CORE_ERROR(
                    "Asset with id '{}' already exists with different type. Expected '{}', got '{}'",
                    id,
                    T::getStaticTypeName(),
                    it->second->getTypeName()
                );
                return nullptr;
            }
            
            // Type matches, return existing asset
            return std::static_pointer_cast<T>(it->second);
        }

        auto loader = getLoader(T::getStaticTypeName());
        if (!loader) {
            STRIKE_CORE_ERROR("No loader found for asset type '{}'", T::getStaticTypeName());
            return nullptr;
        }

        auto placeholder = std::make_shared<T>(id, filePath);
        placeholder->setLoadingState(AssetState::Loading);
        mLoadedAssets[id] = placeholder;

        loader->loadAsync(id, filePath, placeholder);
        return placeholder;
    }

    template<typename T>
    std::shared_ptr<T> AssetManager::getAsset(const std::string& id)
    {
        static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");

        auto it = mLoadedAssets.find(id);
        if (it == mLoadedAssets.end()) {
            return nullptr;
        }

        if (it->second->getTypeName() != T::getStaticTypeName())
        {
            STRIKE_CORE_ERROR(
                "Type mismatch for asset '{}': expected '{}', got '{}'",
                id,
                T::getStaticTypeName(),
                it->second->getTypeName()
            );
            return nullptr;
        }

        return std::static_pointer_cast<T>(it->second);
    }
}
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
        /**
         * @brief Get the singleton instance of the AssetManager.
         * @return Reference to the AssetManager instance.
         */
        static AssetManager& get();

        /**
         * @brief Load an asset of type T synchronously.
         * @param id The unique identifier for the asset.
         * @param src The file path to the asset.
         * @return Shared pointer to the loaded asset of type T.
         */
        template<typename T>
        std::shared_ptr<T> load(const std::string& id, const std::filesystem::path& src);
        
        /**
         * @brief Load an asset of type T asynchronously.
         * @param id The unique identifier for the asset.
         * @param src The file path to the asset.
         * @return Shared pointer to the placeholder asset of type T.
         */
        template<typename T>
        std::shared_ptr<T> loadAsync(const std::string& id, const std::filesystem::path& src);
        
        /**
         * @brief Retrieve a loaded asset of type T by its ID.
         * @param id The unique identifier for the asset.
         * @return Shared pointer to the asset of type T, or nullptr if not found or type mismatch.
         */
        template<typename T>
        std::shared_ptr<T> getAsset(const std::string& id);

        /**
         * @brief Retrieve a loaded asset by its ID as a base Asset pointer.
         * @param id The unique identifier for the asset.
         * @return Shared pointer to the Asset, or nullptr if not found.
         */
        std::shared_ptr<Asset> getAssetBase(const std::string& id) const;

        /**
         * @brief Check if an asset with the given ID exists.
         * @param id The unique identifier for the asset.
         * @return True if the asset exists, false otherwise.
         */
        bool hasAsset(const std::string& id) const;

        /**
         * @brief Check if an asset with the given ID is currently loading.
         * @param id The unique identifier for the asset.
         * @return True if the asset is loading, false otherwise.
         */
        bool isAssetLoading(const std::string& id) const;

        /**
         * @brief Check if the AssetManager is in the process of shutting down.
         * @return True if shutting down, false otherwise.
         */
        bool isShuttingDown() const;

        /**
         * @brief Check if there are any assets currently loading.
         * @return True if any assets are loading, false otherwise.
         */
        bool isLoading() const;

        /**
         * @brief Clear all loaded assets from the manager.
         */
        void clear();

        /**
         * @brief Shutdown the AssetManager, stopping all loading operations.
         */
        void shutdown();

        /**
         * @brief Update the AssetManager, processing any loading operations.
         */
        void update();

        /**
         * @brief Remove an asset by its ID.
         * @param id The unique identifier for the asset.
         */
        void removeAsset(const std::string& id);

        /**
         * @brief Get a list of IDs for all loaded assets.
         * @return Vector of loaded asset IDs.
         */
        std::vector<std::string> getLoadedAssetIds() const;

        /**
         * @brief Get a list of IDs for all assets currently loading.
         * @return Vector of loading asset IDs.
         */
        std::vector<std::string> getLoadingAssetIds() const;

        /**
         * @brief Get the count of loaded assets.
         * @return Number of loaded assets.
         */
        size_t getLoadedAssetCount() const;

        /**
         * @brief Get the count of assets currently loading.
         * @return Number of loading assets.
         */
        size_t getLoadingAssetCount() const;

        /**
         * @brief Deserialize assets from an XML node.
         * @param node The XML node containing asset data.
         */
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
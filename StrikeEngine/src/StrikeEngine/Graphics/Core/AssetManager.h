#pragma once
#include "StrikeEngine/Graphics/Core/Assets/Asset.h"
#include "StrikeEngine/Graphics/Core/AssetLoaders/AssetLoader.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <mutex>
#include <filesystem>
#include <vector>
#include <type_traits>

namespace StrikeEngine {
    class AssetManager {
    public:
        // Singleton access
        static AssetManager& Get() {
            static AssetManager instance;
            return instance;
        }

        // Register a new asset type with its storage
        void RegisterAssetType(AssetType type);

        // Template-based registration (automatically deduces AssetType)
        template<typename AssetT>
        void RegisterLoader(std::unique_ptr<AssetLoader> loader) {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            AssetType type = AssetT::GetStaticAssetType();
            RegisterLoaderByType(type, std::move(loader));
        }

        // Type-based registration
        void RegisterLoaderByType(AssetType type, std::unique_ptr<AssetLoader> loader);

        // Template-based loading (automatically deduces AssetType)
        template<typename AssetT>
        void LoadAsset(const std::string& id, const std::filesystem::path& filePath) {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            AssetType type = AssetT::GetStaticAssetType();
            LoadAssetByType(type, id, filePath);
        }

        template<typename AssetT>
        void LoadAssetAsync(const std::string& id, const std::filesystem::path& filePath) {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            AssetType type = AssetT::GetStaticAssetType();
            LoadAssetByTypeAsync(type, id, filePath);
        }

        // Type-based loading
        void LoadAssetByType(AssetType type, const std::string& id, const std::filesystem::path& filePath);
        void LoadAssetByTypeAsync(AssetType type, const std::string& id, const std::filesystem::path& filePath);

        // Template-based asset retrieval
        template<typename T>
        std::shared_ptr<T> GetAsset(const std::string& id) {
            static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");
            AssetType type = T::GetStaticAssetType();
            auto baseAsset = GetAssetBase(type, id);
            return std::dynamic_pointer_cast<T>(baseAsset);
        }

        // Type-based asset retrieval
        template<typename T>
        std::shared_ptr<T> GetAsset(AssetType type, const std::string& id) {
            static_assert(std::is_base_of_v<Asset, T>, "T must derive from Asset");
            auto baseAsset = GetAssetBase(type, id);
            return std::dynamic_pointer_cast<T>(baseAsset);
        }

        // Utility methods
        bool HasAsset(AssetType type, const std::string& id) const;
        bool IsAssetLoading(AssetType type, const std::string& id) const;
        AssetState GetAssetState(AssetType type, const std::string& id) const;
        void RemoveAsset(AssetType type, const std::string& id);
        void Clear();
        void Update();

        // Asset information methods
        std::vector<std::string> GetLoadedAssetIds(AssetType type) const;
        std::vector<std::string> GetLoadingAssetIds(AssetType type) const;
        size_t GetLoadedAssetCount(AssetType type) const;
        size_t GetLoadingAssetCount(AssetType type) const;

        // Template-based utility methods
        template<typename AssetT>
        bool HasAsset(const std::string& id) const {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            AssetType type = AssetT::GetStaticAssetType();
            return HasAsset(type, id);
        }

        template<typename AssetT>
        bool IsAssetLoading(const std::string& id) const {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            AssetType type = AssetT::GetStaticAssetType();
            return IsAssetLoading(type, id);
        }

        template<typename AssetT>
        AssetState GetAssetState(const std::string& id) const {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            AssetType type = AssetT::GetStaticAssetType();
            return GetAssetState(type, id);
        }

        template<typename AssetT>
        void RemoveAsset(const std::string& id) {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            AssetType type = AssetT::GetStaticAssetType();
            RemoveAsset(type, id);
        }

        template<typename AssetT>
        std::vector<std::string> GetLoadedAssetIds() const {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            AssetType type = AssetT::GetStaticAssetType();
            return GetLoadedAssetIds(type);
        }

        template<typename AssetT>
        std::vector<std::string> GetLoadingAssetIds() const {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            AssetType type = AssetT::GetStaticAssetType();
            return GetLoadingAssetIds(type);
        }

        template<typename AssetT>
        size_t GetLoadedAssetCount() const {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            AssetType type = AssetT::GetStaticAssetType();
            return GetLoadedAssetCount(type);
        }

        template<typename AssetT>
        size_t GetLoadingAssetCount() const {
            static_assert(std::is_base_of_v<Asset, AssetT>, "AssetT must derive from Asset");
            AssetType type = AssetT::GetStaticAssetType();
            return GetLoadingAssetCount(type);
        }

    private:
        AssetManager();
        ~AssetManager();
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;



        void RegisterAssetLoaders();
        std::shared_ptr<Asset> GetAssetBase(AssetType type, const std::string& id);
    private:

        // Structure to hold assets of a specific type
        struct AssetStore {
            std::unordered_map<std::string, std::shared_ptr<Asset>> loadedAssets;
            std::vector<std::string> loadingAssetIds;
        };

        std::unordered_map<AssetType, std::unique_ptr<AssetStore>> m_assetStores;
        std::unordered_map<AssetType, std::unique_ptr<AssetLoader>> m_loaders;
        mutable std::mutex m_mutex;


    };
}
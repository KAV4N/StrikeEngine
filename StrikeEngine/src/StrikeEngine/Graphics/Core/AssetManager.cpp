#include "AssetManager.h"
#include <iostream>
#include <algorithm>

#include "StrikeEngine/Graphics/Core/Assets/Mesh.h"
#include "StrikeEngine/Graphics/Core/AssetLoaders/MeshLoader.h"

namespace StrikeEngine {

    AssetManager::AssetManager() {
        RegisterAssetLoaders();
    }

    void AssetManager::RegisterAssetLoaders() {
        RegisterLoader<Mesh>(std::make_unique<MeshLoader>());
    }

    AssetManager::~AssetManager() { Clear(); }

    void AssetManager::RegisterAssetType(AssetType type) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_assetStores.find(type) == m_assetStores.end()) {
            m_assetStores[type] = std::make_unique<AssetStore>();
        }
    }

    void AssetManager::RegisterLoaderByType(AssetType type, std::unique_ptr<AssetLoader> loader) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_loaders[type] = std::move(loader);
    }

    void AssetManager::LoadAssetByType(AssetType type, const std::string& id, const std::filesystem::path& filePath) {
        std::lock_guard<std::mutex> lock(m_mutex);

        // Ensure asset store exists for this type
        if (m_assetStores.find(type) == m_assetStores.end()) {
            m_assetStores[type] = std::make_unique<AssetStore>();
        }

        auto& store = m_assetStores[type];
        if (store->loadedAssets.find(id) != store->loadedAssets.end() ||
            std::find(store->loadingAssetIds.begin(), store->loadingAssetIds.end(), id) != store->loadingAssetIds.end()) {
            std::cerr << "Asset with ID '" << id << "' already exists or is loading for type " << static_cast<int>(type) << std::endl;
            return;
        }

        auto loaderIt = m_loaders.find(type);
        if (loaderIt == m_loaders.end()) {
            std::cerr << "No loader registered for asset type: " << static_cast<int>(type) << std::endl;
            return;
        }

        try {
            auto asset = loaderIt->second->Load(id, filePath);
            if (asset) {
                store->loadedAssets[id] = asset;
            }
            else {
                std::cerr << "Failed to load asset: " << id << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Asset loading failed: " << e.what() << std::endl;
        }
    }

    void AssetManager::LoadAssetByTypeAsync(AssetType type, const std::string& id, const std::filesystem::path& filePath) {
        std::lock_guard<std::mutex> lock(m_mutex);

        // Ensure asset store exists for this type
        if (m_assetStores.find(type) == m_assetStores.end()) {
            m_assetStores[type] = std::make_unique<AssetStore>();
        }

        auto& store = m_assetStores[type];
        if (store->loadedAssets.find(id) != store->loadedAssets.end() ||
            std::find(store->loadingAssetIds.begin(), store->loadingAssetIds.end(), id) != store->loadingAssetIds.end()) {
            std::cerr << "Asset with ID '" << id << "' already exists or is loading for type " << static_cast<int>(type) << std::endl;
            return;
        }

        auto loaderIt = m_loaders.find(type);
        if (loaderIt == m_loaders.end()) {
            std::cerr << "No loader registered for asset type: " << static_cast<int>(type) << std::endl;
            return;
        }

        loaderIt->second->QueueLoadAsync(id, filePath);
        store->loadingAssetIds.push_back(id);
    }

    std::shared_ptr<Asset> AssetManager::GetAssetBase(AssetType type, const std::string& id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto storeIt = m_assetStores.find(type);
        if (storeIt == m_assetStores.end()) {
            return nullptr;
        }
        auto it = storeIt->second->loadedAssets.find(id);
        return (it != storeIt->second->loadedAssets.end()) ? it->second : nullptr;
    }

    bool AssetManager::HasAsset(AssetType type, const std::string& id) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto storeIt = m_assetStores.find(type);
        if (storeIt == m_assetStores.end()) {
            return false;
        }
        return storeIt->second->loadedAssets.find(id) != storeIt->second->loadedAssets.end();
    }

    bool AssetManager::IsAssetLoading(AssetType type, const std::string& id) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto storeIt = m_assetStores.find(type);
        if (storeIt == m_assetStores.end()) {
            return false;
        }
        return std::find(storeIt->second->loadingAssetIds.begin(),
            storeIt->second->loadingAssetIds.end(), id) != storeIt->second->loadingAssetIds.end();
    }

    AssetState AssetManager::GetAssetState(AssetType type, const std::string& id) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto storeIt = m_assetStores.find(type);
        if (storeIt == m_assetStores.end()) {
            return AssetState::UNINITIALIZED;
        }

        if (storeIt->second->loadedAssets.find(id) != storeIt->second->loadedAssets.end()) {
            return AssetState::READY;
        }

        if (std::find(storeIt->second->loadingAssetIds.begin(),
            storeIt->second->loadingAssetIds.end(), id) != storeIt->second->loadingAssetIds.end()) {
            return AssetState::LOADING;
        }

        return AssetState::UNINITIALIZED;
    }

    void AssetManager::RemoveAsset(AssetType type, const std::string& id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto storeIt = m_assetStores.find(type);
        if (storeIt == m_assetStores.end()) {
            return;
        }

        storeIt->second->loadedAssets.erase(id);
        auto it = std::find(storeIt->second->loadingAssetIds.begin(),
            storeIt->second->loadingAssetIds.end(), id);
        if (it != storeIt->second->loadingAssetIds.end()) {
            storeIt->second->loadingAssetIds.erase(it);
        }
    }

    void AssetManager::Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_assetStores.clear();
        for (auto& [type, loader] : m_loaders) {
            while (loader->GetLoadingAssetCount() > 0) {
                loader->GetCompletedAssets();
            }
        }
    }

    void AssetManager::Update() {
        std::lock_guard<std::mutex> lock(m_mutex);

        for (auto& [type, loader] : m_loaders) {
            auto completedAssets = loader->GetCompletedAssets();
            if (!completedAssets.empty()) {
                if (m_assetStores.find(type) == m_assetStores.end()) {
                    m_assetStores[type] = std::make_unique<AssetStore>();
                }
                auto& store = m_assetStores[type];
                for (auto& [id, asset] : completedAssets) {
                    if (asset && store->loadedAssets.find(id) == store->loadedAssets.end()) {
                        store->loadedAssets[id] = asset;
                        auto it = std::find(store->loadingAssetIds.begin(),
                            store->loadingAssetIds.end(), id);
                        if (it != store->loadingAssetIds.end()) {
                            store->loadingAssetIds.erase(it);
                        }
                    }
                }
            }
        }
    }

    std::vector<std::string> AssetManager::GetLoadedAssetIds(AssetType type) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<std::string> loadedIds;
        auto storeIt = m_assetStores.find(type);
        if (storeIt == m_assetStores.end()) {
            return loadedIds;
        }

        loadedIds.reserve(storeIt->second->loadedAssets.size());
        for (const auto& pair : storeIt->second->loadedAssets) {
            loadedIds.push_back(pair.first);
        }
        return loadedIds;
    }

    std::vector<std::string> AssetManager::GetLoadingAssetIds(AssetType type) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto storeIt = m_assetStores.find(type);
        if (storeIt == m_assetStores.end()) {
            return std::vector<std::string>();
        }
        return storeIt->second->loadingAssetIds;
    }

    size_t AssetManager::GetLoadedAssetCount(AssetType type) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto storeIt = m_assetStores.find(type);
        if (storeIt == m_assetStores.end()) {
            return 0;
        }
        return storeIt->second->loadedAssets.size();
    }

    size_t AssetManager::GetLoadingAssetCount(AssetType type) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto storeIt = m_assetStores.find(type);
        if (storeIt == m_assetStores.end()) {
            return 0;
        }
        return storeIt->second->loadingAssetIds.size();
    }

}
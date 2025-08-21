#include "AssetManager.h"
#include "StrikeEngine/Asset/Loaders/MeshLoader.h"

namespace StrikeEngine {

    AssetManager& AssetManager::get() {
        static AssetManager instance;
        return instance;
    }

    AssetManager::AssetManager() {
        registerAssetLoaders();
    }

    void AssetManager::registerAssetLoaders() {
        mLoaders[Mesh::getStaticTypeName()] = std::make_unique<MeshLoader>();
    }

    AssetManager::~AssetManager() = default;


    std::shared_ptr<Asset> AssetManager::getAsset(const std::string& type, const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);

        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            if (auto asset = it->second.lock()) {
                if (asset->getTypeName() == type) {
                    return asset;
                }
            }
            mLoadedAssets.erase(it); 
        }
        return nullptr;
    }

    std::shared_ptr<Asset> AssetManager::loadAsset(const std::string& type, const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingAsset = getAsset(type, id)) {
            return existingAsset;
        }

        auto loader = getLoader(type);
        if (!loader) {
            return nullptr;
        }

        return loader->load(id, filePath);
    }


    std::shared_ptr<Asset> AssetManager::loadAssetAsync(const std::string& type, const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingAsset = getAsset(type, id)) {
            return existingAsset;
        }

        auto loader = getLoader(type);
        if (!loader) {
            return nullptr;
        }

        auto placeholderAsset = loader->createPlaceholder(id, filePath);
        
        if (!placeholderAsset) {
            return nullptr;
        }

        placeholderAsset->setLoadingState(AssetLoadingState::Loading);


        placeholderAsset->setName(filePath.stem().string());

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = placeholderAsset;
        }

        loader->loadAsync(id, filePath, placeholderAsset);

        return placeholderAsset;
    }


    bool AssetManager::hasAsset(const std::string& id) const {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        return it != mLoadedAssets.end() && !it->second.expired();
    }

    bool AssetManager::isAssetLoading(const std::string& id) const {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            if (auto asset = it->second.lock()) {
                return asset->isLoading();
            }
        }
        return false;
    }

    void AssetManager::clear() {
        std::lock_guard<std::mutex> lock(mMutex);
        mLoadedAssets.clear();
        for (auto& [type, loader] : mLoaders) {
            loader->clearLoadingTasks();
        }
    }

    void AssetManager::update() {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            auto it = mLoadedAssets.begin();
            while (it != mLoadedAssets.end()) {
                if (it->second.expired()) {
                    it = mLoadedAssets.erase(it);
                }
                else {
                    ++it;
                }
            }
        }

        for (auto& [type, loader] : mLoaders) {
            loader->update();
        }
    }

    std::vector<std::string> AssetManager::getLoadedAssetIds() const {
        std::lock_guard<std::mutex> lock(mMutex);
        std::vector<std::string> ids;
        for (const auto& [id, weakAsset] : mLoadedAssets) {
            if (auto asset = weakAsset.lock()) {
                if (asset->isReady()) {
                    ids.push_back(id);
                }
            }
        }
        return ids;
    }

    std::vector<std::string> AssetManager::getLoadingAssetIds() const {
        std::lock_guard<std::mutex> lock(mMutex);
        std::vector<std::string> ids;
        for (const auto& [id, weakAsset] : mLoadedAssets) {
            if (auto asset = weakAsset.lock()) {
                if (asset->isLoading()) {
                    ids.push_back(id);
                }
            }
        }
        return ids;
    }

    size_t AssetManager::getLoadedAssetCount() const {
        std::lock_guard<std::mutex> lock(mMutex);
        size_t count = 0;
        for (const auto& [id, weakAsset] : mLoadedAssets) {
            if (auto asset = weakAsset.lock()) {
                if (asset->isReady()) {
                    count++;
                }
            }
        }
        return count;
    }

    size_t AssetManager::getLoadingAssetCount() const {
        std::lock_guard<std::mutex> lock(mMutex);
        size_t count = 0;
        for (const auto& [id, weakAsset] : mLoadedAssets) {
            if (auto asset = weakAsset.lock()) {
                if (asset->isLoading()) {
                    count++;
                }
            }
        }
        return count;
    }

    std::shared_ptr<Asset> AssetManager::getAssetBase(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            return it->second.lock();
        }
        return nullptr;
    }

    void AssetManager::removeAssetInternal(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        mLoadedAssets.erase(id);
    }

    AssetLoader* AssetManager::getLoader(const std::string& typeName) {
        auto it = mLoaders.find(typeName);
        return (it != mLoaders.end()) ? it->second.get() : nullptr;
    }


} // namespace StrikeEngine

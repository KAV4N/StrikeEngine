#include "strikepch.h"
#include "AssetManager.h"

#include "StrikeEngine/Asset/Loaders/AssetLoader.h"
#include "StrikeEngine/Asset/Loaders/ModelLoader.h"
#include "StrikeEngine/Asset/Loaders/TemplateLoader.h"
#include "StrikeEngine/Asset/Loaders/TextureLoader.h"
#include "StrikeEngine/Asset/Loaders/AudioLoader.h"

#include "ModelParser.h"

namespace Strike {

    AssetManager& AssetManager::get() {
        static AssetManager instance;
        return instance;
    }

    AssetManager::AssetManager() {
        registerAssetLoaders();
    }
    
    AssetManager::~AssetManager() {
        shutdown();
    }

    void AssetManager::registerAssetLoaders() {
        mLoaders[Model::getStaticTypeName()]    = std::make_unique<ModelLoader>();
        mLoaders[Template::getStaticTypeName()] = std::make_unique<TemplateLoader>();
        mLoaders[Texture::getStaticTypeName()]  = std::make_unique<TextureLoader>();
        mLoaders[CubeMap::getStaticTypeName()]  = std::make_unique<CubeMapLoader>();
        mLoaders[Audio::getStaticTypeName()]    = std::make_unique<AudioLoader>();
    }

    std::shared_ptr<Asset> AssetManager::getAssetBase(const std::string& id) const {
        auto it = mLoadedAssets.find(id);
        return (it != mLoadedAssets.end()) ? it->second : nullptr;
    }

    bool AssetManager::hasAsset(const std::string& id) const {
        return mLoadedAssets.find(id) != mLoadedAssets.end();
    }

    bool AssetManager::isAssetLoading(const std::string& id) const {
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            return it->second->isLoading();
        }
        return false;
    }

    bool AssetManager::isLoading() const {
        for (const auto& [type, loader] : mLoaders) {
            if (loader->hasLoadingTasks()) {
                return true;
            }
        }
        return false;
    }

    bool AssetManager::isShuttingDown() const {
        return mShuttingDown;
    }

    void AssetManager::clear() {
        mShuttingDown = true;

        for (auto& [type, loader] : mLoaders) {
            loader->clearLoadingTasks();
        }

        mLoadedAssets.clear();

        mShuttingDown = false;
    }

    void AssetManager::shutdown() {
        if (mShuttingDown) {
            return;
        }

        mShuttingDown = true;

        for (auto& [type, loader] : mLoaders) {
            loader->clearLoadingTasks();
        }
        mLoadedAssets.clear();
        mLoaders.clear();
    }

    void AssetManager::update() {
        if (mShuttingDown) {
            return;
        }

        for (auto& [type, loader] : mLoaders) {
            loader->update();
        }
    }

    void AssetManager::removeAsset(const std::string& id) {
        mLoadedAssets.erase(id);
    }

    std::vector<std::string> AssetManager::getLoadedAssetIds() const {
        std::vector<std::string> ids;
        ids.reserve(mLoadedAssets.size());
        for (const auto& [id, asset] : mLoadedAssets) {
            if (asset->isReady()) {
                ids.push_back(id);
            }
        }
        return ids;
    }

    std::vector<std::string> AssetManager::getLoadingAssetIds() const {
        std::vector<std::string> ids;
        ids.reserve(mLoadedAssets.size());
        for (const auto& [id, asset] : mLoadedAssets) {
            if (asset->isLoading()) {
                ids.push_back(id);
            }
        }
        return ids;
    }

    size_t AssetManager::getLoadedAssetCount() const {
        size_t count = 0;
        for (const auto& [id, asset] : mLoadedAssets) {
            if (asset->isReady()) {
                ++count;
            }
        }
        return count;
    }

    size_t AssetManager::getLoadingAssetCount() const {
        size_t count = 0;
        for (const auto& [id, asset] : mLoadedAssets) {
            if (asset->isLoading()) {
                ++count;
            }
        }
        return count;
    }

    void AssetManager::removeAssetInternal(const std::string& id) {
        mLoadedAssets.erase(id);
    }

    AssetLoader* AssetManager::getLoader(const std::string& typeName) {
        auto it = mLoaders.find(typeName);
        return (it != mLoaders.end()) ? it->second.get() : nullptr;
    }

    std::shared_ptr<Asset> AssetManager::loadInternal(const std::string& id, std::filesystem::path filePath, const std::string& assetType, std::shared_ptr<Asset> placeholder, bool async) {
        auto loader = getLoader(assetType);
        STRIKE_ASSERT(loader, "No loader found for asset type '{}'", assetType);

        if (assetType == Template::getStaticTypeName()) {
            std::filesystem::path tmplPath = filePath;
            tmplPath.replace_extension(".tmpl");

            if (!std::filesystem::exists(tmplPath)) {
                ModelParser parser;
                if (!parser.parseModel(filePath)) {
                    placeholder->setState(AssetState::Failed);
                    return placeholder;
                }
            }

            filePath = tmplPath;
        }

        placeholder->setState(AssetState::Loading);
        mLoadedAssets[id] = placeholder;

        if (async) {
            loader->loadAsync(id, filePath, placeholder);
            return placeholder;
        } else {
            return loader->load(id, filePath);
        }
    }

    void AssetManager::deserialize(const pugi::xml_node& node, const std::filesystem::path& basePath, bool direct) {
        if (mShuttingDown) {
            return;
        }

        if (direct) {
            const std::string assetId = node.attribute("id").as_string();

            auto it = mLoadedAssets.find(assetId);
            if (it != mLoadedAssets.end()) {
                STRIKE_ASSERT(it->second->getTypeName() == node.name(),
                    "Cannot deserialize asset '{}': already exists with different type. Expected '{}', got '{}'",
                    assetId, node.name(), it->second->getTypeName()
                );
                return;
            }

            auto loader = getLoader(node.name());
            STRIKE_ASSERT(loader, "No loader found for asset type '{}'", node.name());

            auto asset = loader->loadFromNode(node, basePath);
            if (asset) {
                mLoadedAssets[asset->getId()] = asset;
            }
        } else {
            for (const pugi::xml_node& assetNode : node.children()) {
                const std::string assetId = assetNode.attribute("id").as_string();

                auto it = mLoadedAssets.find(assetId);
                if (it != mLoadedAssets.end()) {
                    STRIKE_ASSERT(it->second->getTypeName() == assetNode.name(),
                        "Cannot deserialize asset '{}': already exists with different type. Expected '{}', got '{}'",
                        assetId, assetNode.name(), it->second->getTypeName()
                    );
                    continue;
                }

                auto loader = getLoader(assetNode.name());
                STRIKE_ASSERT(loader, "No loader found for asset type '{}'", assetNode.name());

                auto asset = loader->loadFromNode(assetNode, basePath);
                if (asset) {
                    mLoadedAssets[asset->getId()] = asset;
                }
            }
        }
    }

}
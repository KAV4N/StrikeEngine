
#include "strikepch.h"
#include "AssetManager.h"

#include "StrikeEngine/Asset/Loaders/AssetLoader.h"
#include "StrikeEngine/Asset/Loaders/ModelLoader.h"
#include "StrikeEngine/Asset/Loaders/TemplateLoader.h"
#include "StrikeEngine/Asset/Loaders/TextureLoader.h"
#include "StrikeEngine/Asset/Loaders/MaterialLoader.h"
#include "StrikeEngine/Asset/Loaders/AudioLoader.h"

namespace StrikeEngine {

    AssetManager& AssetManager::get() {
        static AssetManager instance;
        return instance;
    }

    AssetManager::AssetManager() {
        registerAssetLoaders();
    }
    AssetManager::~AssetManager() = default;


    void AssetManager::registerAssetLoaders() {
        mLoaders[Model::getStaticTypeName()] = std::make_unique<ModelLoader>();
        mLoaders[Template::getStaticTypeName()] = std::make_unique<TemplateLoader>();
        mLoaders[Texture::getStaticTypeName()] = std::make_unique<TextureLoader>();
        mLoaders[CubeMap::getStaticTypeName()] = std::make_unique<CubeMapLoader>();
        mLoaders[Material::getStaticTypeName()] = std::make_unique<MaterialLoader>();
        mLoaders[Audio::getStaticTypeName()] = std::make_unique<AudioLoader>();
    }




    std::shared_ptr<Model> AssetManager::loadModel(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingModel = getModel(id)) {
            return existingModel;
        }

        auto loader = getLoader(Model::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto loadedAsset = loader->load(id, filePath);
        if (loadedAsset) {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = loadedAsset;
        }
        return std::static_pointer_cast<Model>(loadedAsset);
    }

    std::shared_ptr<Model> AssetManager::loadModelAsync(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingModel = getModel(id)) {
            return existingModel;
        }

        auto loader = getLoader(Model::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto placeholderModel = std::make_shared<Model>(id, filePath);
        placeholderModel->setLoadingState(AssetLoadingState::Loading);

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = placeholderModel;
        }

        loader->loadAsync(id, filePath, placeholderModel);
        return placeholderModel;
    }

    std::shared_ptr<Model> AssetManager::getModel(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            if (it->second->getTypeName() == Model::getStaticTypeName()) {
                return std::static_pointer_cast<Model>(it->second);
            }
            mLoadedAssets.erase(it);
        }
        return nullptr;
    }

    std::shared_ptr<Template> AssetManager::loadTemplate(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingTemplate = getTemplate(id)) {
            return existingTemplate;
        }

        auto loader = getLoader(Template::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto loadedAsset = loader->load(id, filePath);
        if (loadedAsset) {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = loadedAsset;
        }
        return std::static_pointer_cast<Template>(loadedAsset);
    }

    std::shared_ptr<Template> AssetManager::loadTemplateAsync(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingTemplate = getTemplate(id)) {
            return existingTemplate;
        }

        auto loader = getLoader(Template::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto placeholderTemplate = std::make_shared<Template>(id, filePath);
        placeholderTemplate->setLoadingState(AssetLoadingState::Loading);

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = placeholderTemplate;
        }

        loader->loadAsync(id, filePath, placeholderTemplate);
        return placeholderTemplate;
    }

    std::shared_ptr<Template> AssetManager::getTemplate(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            if (it->second->getTypeName() == Template::getStaticTypeName()) {
                return std::static_pointer_cast<Template>(it->second);
            }
            mLoadedAssets.erase(it);
        }
        return nullptr;
    }

    std::shared_ptr<Texture> AssetManager::loadTexture(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingTexture = getTexture(id)) {
            return existingTexture;
        }

        auto loader = getLoader(Texture::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto loadedAsset = loader->load(id, filePath);
        if (loadedAsset) {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = loadedAsset;
        }
        return std::static_pointer_cast<Texture>(loadedAsset);
    }

    std::shared_ptr<Texture> AssetManager::loadTextureAsync(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingTexture = getTexture(id)) {
            return existingTexture;
        }

        auto loader = getLoader(Texture::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto placeholderTexture = std::make_shared<Texture>(id, filePath);
        placeholderTexture->setLoadingState(AssetLoadingState::Loading);

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = placeholderTexture;
        }

        loader->loadAsync(id, filePath, placeholderTexture);
        return placeholderTexture;
    }

    std::shared_ptr<Texture> AssetManager::getTexture(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            if (it->second->getTypeName() == Texture::getStaticTypeName()) {
                return std::static_pointer_cast<Texture>(it->second);
            }
            mLoadedAssets.erase(it);
        }
        return nullptr;
    }

    std::shared_ptr<CubeMap> AssetManager::loadCubeMap(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingCubeMap = getCubeMap(id)) {
            return existingCubeMap;
        }

        auto loader = getLoader(CubeMap::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto loadedAsset = loader->load(id, filePath);
        if (loadedAsset) {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = loadedAsset;
        }
        return std::static_pointer_cast<CubeMap>(loadedAsset);
    }

    std::shared_ptr<CubeMap> AssetManager::loadCubeMapAsync(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingCubeMap = getCubeMap(id)) {
            return existingCubeMap;
        }

        auto loader = getLoader(CubeMap::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto placeholderCubeMap = std::make_shared<CubeMap>(id, filePath);
        placeholderCubeMap->setLoadingState(AssetLoadingState::Loading);

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = placeholderCubeMap;
        }

        loader->loadAsync(id, filePath, placeholderCubeMap);
        return placeholderCubeMap;
    }

    std::shared_ptr<CubeMap> AssetManager::getCubeMap(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            if (it->second->getTypeName() == CubeMap::getStaticTypeName()) {
                return std::static_pointer_cast<CubeMap>(it->second);
            }
            mLoadedAssets.erase(it);
        }
        return nullptr;
    }


    std::shared_ptr<Audio> AssetManager::loadAudio(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingAudio = getAudio(id)) {
            return existingAudio;
        }

        auto loader = getLoader(Audio::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto loadedAsset = loader->load(id, filePath);
        if (loadedAsset) {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = loadedAsset;
        }
        return std::static_pointer_cast<Audio>(loadedAsset);
    }

    std::shared_ptr<Audio> AssetManager::loadAudioAsync(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingAudio = getAudio(id)) {
            return existingAudio;
        }

        auto loader = getLoader(Audio::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto placeholderAudio = std::make_shared<Audio>(id, filePath);
        placeholderAudio->setLoadingState(AssetLoadingState::Loading);

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = placeholderAudio;
        }

        loader->loadAsync(id, filePath, placeholderAudio);
        return placeholderAudio;
    }

    std::shared_ptr<Audio> AssetManager::getAudio(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            if (it->second->getTypeName() == Audio::getStaticTypeName()) {
                return std::static_pointer_cast<Audio>(it->second);
            }
            mLoadedAssets.erase(it);
        }
        return nullptr;
    }


    std::shared_ptr<Material> AssetManager::loadMaterial(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingMaterial = getMaterial(id)) {
            return existingMaterial;
        }

        auto loader = getLoader(Material::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto loadedAsset = loader->load(id, filePath);
        if (loadedAsset) {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = loadedAsset;
        }
        return std::static_pointer_cast<Material>(loadedAsset);
    }

    std::shared_ptr<Material> AssetManager::loadMaterialAsync(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingMaterial = getMaterial(id)) {
            return existingMaterial;
        }

        auto loader = getLoader(Material::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto placeholderMaterial = std::make_shared<Material>(id, filePath);
        placeholderMaterial->setLoadingState(AssetLoadingState::Loading);

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = placeholderMaterial;
        }

        loader->loadAsync(id, filePath, placeholderMaterial);
        return placeholderMaterial;
    }

    std::shared_ptr<Material> AssetManager::getMaterial(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            if (it->second->getTypeName() == Material::getStaticTypeName()) {
                return std::static_pointer_cast<Material>(it->second);
            }
            mLoadedAssets.erase(it);
        }
        return nullptr;
    }

    bool AssetManager::hasAsset(const std::string& id) const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mLoadedAssets.find(id) != mLoadedAssets.end();
    }

    bool AssetManager::isAssetLoading(const std::string& id) const {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            return it->second->isLoading();
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
        for (auto& [type, loader] : mLoaders) {
            loader->update();
        }
    }

    void AssetManager::removeAsset(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        mLoadedAssets.erase(id);
    }

    std::vector<std::string> AssetManager::getLoadedAssetIds() const {
        std::lock_guard<std::mutex> lock(mMutex);
        std::vector<std::string> ids;
        for (const auto& [id, asset] : mLoadedAssets) {
            if (asset->isReady()) {
                ids.push_back(id);
            }
        }
        return ids;
    }

    std::vector<std::string> AssetManager::getLoadingAssetIds() const {
        std::lock_guard<std::mutex> lock(mMutex);
        std::vector<std::string> ids;
        for (const auto& [id, asset] : mLoadedAssets) {
            if (asset->isLoading()) {
                ids.push_back(id);
            }
        }
        return ids;
    }

    size_t AssetManager::getLoadedAssetCount() const {
        std::lock_guard<std::mutex> lock(mMutex);
        size_t count = 0;
        for (const auto& [id, asset] : mLoadedAssets) {
            if (asset->isReady()) {
                count++;
            }
        }
        return count;
    }

    size_t AssetManager::getLoadingAssetCount() const {
        std::lock_guard<std::mutex> lock(mMutex);
        size_t count = 0;
        for (const auto& [id, asset] : mLoadedAssets) {
            if (asset->isLoading()) {
                count++;
            }
        }
        return count;
    }

    std::shared_ptr<Asset> AssetManager::getAssetBase(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            return it->second;
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

    void AssetManager::serialize(pugi::xml_document& doc) {
        pugi::xml_node assetsNode = doc.append_child("assets");

        std::lock_guard<std::mutex> lock(mMutex);
        for (const auto& [id, asset] : mLoadedAssets) {
            pugi::xml_node assetNode = asset->toNode();
            assetsNode.append_copy(assetNode);
        }
    }

    void AssetManager::deserialize(const pugi::xml_node& node, const std::filesystem::path& basePath, const bool direct) {
        if (direct){
            if (hasAsset(node.attribute("id").as_string())) return;
            std::string typeName = node.name();
            auto loader = getLoader(typeName);
            if (loader) {
                auto asset = loader->loadFromNode(node, basePath);
                if (asset) {
                    std::lock_guard<std::mutex> lock(mMutex);
                    mLoadedAssets[asset->getId()] = asset;
                }
            }
        }
        else{
            for (pugi::xml_node assetNode : node.children()) {
                if (hasAsset(assetNode.attribute("id").as_string())) return;
                std::string typeName = assetNode.name();
                auto loader = getLoader(typeName);
                if (loader) {
                    auto asset = loader->loadFromNode(assetNode, basePath);
                    if (asset) {
                        std::lock_guard<std::mutex> lock(mMutex);
                        mLoadedAssets[asset->getId()] = asset;
                    }
                }
            }
        }
        
    }
}
#include "AssetManager.h"
#include "StrikeEngine/Asset/Loaders/MeshLoader.h"
#include "StrikeEngine/Asset/Loaders/TemplateLoader.h"
#include "StrikeEngine/Asset/Loaders/ShaderLoader.h"
#include "StrikeEngine/Asset/Loaders/TextureLoader.h"
#include "StrikeEngine/Asset/Loaders/MaterialLoader.h"

#include "StrikeEngine/Asset/Types/Mesh.h"
#include "StrikeEngine/Asset/Types/Shader.h"
#include "StrikeEngine/Asset/Types/Template.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Asset/Types/Material.h"

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
        mLoaders[Template::getStaticTypeName()] = std::make_unique<TemplateLoader>();
        mLoaders[Shader::getStaticTypeName()] = std::make_unique<ShaderLoader>();
        mLoaders[Texture2D::getStaticTypeName()] = std::make_unique<Texture2DLoader>();
        mLoaders[CubeMap::getStaticTypeName()] = std::make_unique<CubeMapLoader>();
        mLoaders[Material::getStaticTypeName()] = std::make_unique<MaterialLoader>();
    }

    AssetManager::~AssetManager() = default;

    std::shared_ptr<Mesh> AssetManager::loadMesh(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingMesh = getMesh(id)) {
            return existingMesh;
        }

        auto loader = getLoader(Mesh::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto loadedAsset = loader->load(id, filePath);
        if (loadedAsset) {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = loadedAsset;
        }
        return std::static_pointer_cast<Mesh>(loadedAsset);
    }

    std::shared_ptr<Mesh> AssetManager::loadMeshAsync(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingMesh = getMesh(id)) {
            return existingMesh;
        }

        auto loader = getLoader(Mesh::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto placeholderMesh = std::make_shared<Mesh>(id, filePath, filePath.stem().string());
        placeholderMesh->setLoadingState(AssetLoadingState::Loading);

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = placeholderMesh;
        }

        loader->loadAsync(id, filePath, placeholderMesh);
        return placeholderMesh;
    }

    std::shared_ptr<Mesh> AssetManager::getMesh(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            if (auto asset = it->second.lock()) {
                if (asset->getTypeName() == Mesh::getStaticTypeName()) {
                    return std::static_pointer_cast<Mesh>(asset);
                }
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

        auto placeholderTemplate = std::make_shared<Template>(id, filePath, filePath.stem().string());
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
            if (auto asset = it->second.lock()) {
                if (asset->getTypeName() == Template::getStaticTypeName()) {
                    return std::static_pointer_cast<Template>(asset);
                }
            }
            mLoadedAssets.erase(it);
        }
        return nullptr;
    }

    std::shared_ptr<Shader> AssetManager::loadShader(const std::string& id, const std::filesystem::path& vertexSrc, const std::filesystem::path& fragmentSrc) {
        if (auto existingShader = getShader(id)) {
            return existingShader;
        }

        auto loader = static_cast<ShaderLoader*>(getLoader(Shader::getStaticTypeName()));
        if (!loader) {
            return nullptr;
        }

        auto loadedAsset = loader->loadShader(id, vertexSrc, fragmentSrc);
        if (loadedAsset) {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = loadedAsset;
        }
        return std::static_pointer_cast<Shader>(loadedAsset);
    }

    std::shared_ptr<Shader> AssetManager::loadShaderAsync(const std::string& id, const std::filesystem::path& vertexSrc, const std::filesystem::path& fragmentSrc) {
        if (auto existingShader = getShader(id)) {
            return existingShader;
        }

        auto loader = static_cast<ShaderLoader*>(getLoader(Shader::getStaticTypeName()));
        if (!loader) {
            return nullptr;
        }

        auto placeholderShader = std::make_shared<Shader>(id, vertexSrc, fragmentSrc, vertexSrc.stem().string());
        placeholderShader->setLoadingState(AssetLoadingState::Loading);

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = placeholderShader;
        }

        loader->loadShaderAsync(id, vertexSrc, fragmentSrc, placeholderShader);
        return placeholderShader;
    }

    std::shared_ptr<Shader> AssetManager::getShader(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            if (auto asset = it->second.lock()) {
                if (asset->getTypeName() == Shader::getStaticTypeName()) {
                    return std::static_pointer_cast<Shader>(asset);
                }
            }
            mLoadedAssets.erase(it);
        }
        return nullptr;
    }

    std::shared_ptr<Texture2D> AssetManager::loadTexture(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingTexture = getTexture(id)) {
            return existingTexture;
        }

        auto loader = getLoader(Texture2D::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto loadedAsset = loader->load(id, filePath);
        if (loadedAsset) {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = loadedAsset;
        }
        return std::static_pointer_cast<Texture2D>(loadedAsset);
    }

    std::shared_ptr<Texture2D> AssetManager::loadTextureAsync(const std::string& id, const std::filesystem::path& filePath) {
        if (auto existingTexture = getTexture(id)) {
            return existingTexture;
        }

        auto loader = getLoader(Texture2D::getStaticTypeName());
        if (!loader) {
            return nullptr;
        }

        auto placeholderTexture = std::make_shared<Texture2D>(id, filePath, filePath.stem().string());
        placeholderTexture->setLoadingState(AssetLoadingState::Loading);

        {
            std::lock_guard<std::mutex> lock(mMutex);
            mLoadedAssets[id] = placeholderTexture;
        }

        loader->loadAsync(id, filePath, placeholderTexture);
        return placeholderTexture;
    }

    std::shared_ptr<Texture2D> AssetManager::getTexture(const std::string& id) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadedAssets.find(id);
        if (it != mLoadedAssets.end()) {
            if (auto asset = it->second.lock()) {
                if (asset->getTypeName() == Texture2D::getStaticTypeName()) {
                    return std::static_pointer_cast<Texture2D>(asset);
                }
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

        auto placeholderCubeMap = std::make_shared<CubeMap>(id, filePath, filePath.stem().string());
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
            if (auto asset = it->second.lock()) {
                if (asset->getTypeName() == CubeMap::getStaticTypeName()) {
                    return std::static_pointer_cast<CubeMap>(asset);
                }
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

        auto placeholderMaterial = std::make_shared<Material>(id, filePath, filePath.stem().string());
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
            if (auto asset = it->second.lock()) {
                if (asset->getTypeName() == Material::getStaticTypeName()) {
                    return std::static_pointer_cast<Material>(asset);
                }
            }
            mLoadedAssets.erase(it);
        }
        return nullptr;
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

    void AssetManager::serialize(pugi::xml_document& doc) {
        pugi::xml_node assetsNode = doc.append_child("assets");

        std::lock_guard<std::mutex> lock(mMutex);
        for (const auto& [id, weakAsset] : mLoadedAssets) {
            if (auto asset = weakAsset.lock()) {
                pugi::xml_node assetNode = asset->toNode();
                assetsNode.append_copy(assetNode);
            }
        }
    }

    void AssetManager::deserialize(const pugi::xml_node& node, std::unordered_map<std::string, std::shared_ptr<Asset>>& assets, const std::filesystem::path& basePath) {
        for (pugi::xml_node assetNode : node.children()) {
            std::string typeName = assetNode.name();
            auto loader = getLoader(typeName);
            if (loader) {
                auto asset = loader->loadFromNode(assetNode, basePath);
                if (asset) {
                    std::lock_guard<std::mutex> lock(mMutex);
                    mLoadedAssets[asset->getId()] = asset;
                    assets.emplace(asset->getId(), asset);
                }
            }
        }
    }
}
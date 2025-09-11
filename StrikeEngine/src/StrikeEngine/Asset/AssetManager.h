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
    class Mesh;
    class Shader;
    class Template;
    class Texture2D;
    class Material;

    class AssetManager {
    public:
        static AssetManager& get();

        // Mesh-specific methods
        std::shared_ptr<Mesh> loadMesh(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Mesh> loadMeshAsync(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Mesh> getMesh(const std::string& id);

        // Template-specific methods
        std::shared_ptr<Template> loadTemplate(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Template> loadTemplateAsync(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Template> getTemplate(const std::string& id);

        // Shader-specific methods
        std::shared_ptr<Shader> loadShader(const std::string& id, const std::filesystem::path& vertexSrc, const std::filesystem::path& fragmentSrc);
        std::shared_ptr<Shader> loadShaderAsync(const std::string& id, const std::filesystem::path& vertexSrc, const std::filesystem::path& fragmentSrc);
        std::shared_ptr<Shader> getShader(const std::string& id);

        // Texture2D-specific methods
        std::shared_ptr<Texture2D> loadTexture(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Texture2D> loadTextureAsync(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Texture2D> getTexture(const std::string& id);

        // Material-specific methods
        std::shared_ptr<Material> loadMaterial(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Material> loadMaterialAsync(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Material> getMaterial(const std::string& id);

        bool hasAsset(const std::string& id) const;
        bool isAssetLoading(const std::string& id) const;

        void clear();
        void update();

        std::vector<std::string> getLoadedAssetIds() const;
        std::vector<std::string> getLoadingAssetIds() const;

        size_t getLoadedAssetCount() const;
        size_t getLoadingAssetCount() const;

        void deserialize(const pugi::xml_node& node, std::vector<std::shared_ptr<Asset>>& assets);
        void serialize(pugi::xml_document& doc);

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
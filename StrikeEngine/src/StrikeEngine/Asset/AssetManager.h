#pragma once

#include "StrikeEngine/Asset/Types/Asset.h"
#include "StrikeEngine/Asset/Types/Model.h"
#include "StrikeEngine/Asset/Types/Template.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Asset/Types/Material.h"
#include "StrikeEngine/Asset/Types/Audio.h"

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
    
    class AssetManager {
    public:
        static AssetManager& get();

        std::shared_ptr<Model> loadModel(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Model> loadModelAsync(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Model> getModel(const std::string& id);

        std::shared_ptr<Template> loadTemplate(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Template> loadTemplateAsync(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Template> getTemplate(const std::string& id);

        std::shared_ptr<Texture> loadTexture(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Texture> loadTextureAsync(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Texture> getTexture(const std::string& id);

        std::shared_ptr<CubeMap> loadCubeMap(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<CubeMap> loadCubeMapAsync(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<CubeMap> getCubeMap(const std::string& id);

        std::shared_ptr<Material> loadMaterial(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Material> loadMaterialAsync(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Material> getMaterial(const std::string& id);

        std::shared_ptr<Audio> loadAudio(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Audio> loadAudioAsync(const std::string& id, const std::filesystem::path& src);
        std::shared_ptr<Audio> getAudio(const std::string& id);

        bool hasAsset(const std::string& id) const;
        bool isAssetLoading(const std::string& id) const;

        void clear();
        void update();
        void removeAsset(const std::string& id);

        std::vector<std::string> getLoadedAssetIds() const;
        std::vector<std::string> getLoadingAssetIds() const;

        size_t getLoadedAssetCount() const;
        size_t getLoadingAssetCount() const;

        void deserialize(const pugi::xml_node& node, const std::filesystem::path& basePath, const bool direct = false);
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
        friend class SceneLoader;
        friend class TemplateLoader;

        std::unordered_map<std::string, std::shared_ptr<Asset>> mLoadedAssets;
        std::unordered_map<std::string, std::unique_ptr<AssetLoader>> mLoaders;
        mutable std::mutex mMutex;
    };

}
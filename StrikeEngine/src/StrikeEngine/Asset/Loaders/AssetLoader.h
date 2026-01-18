#pragma once

#include "StrikeEngine/Asset/Types/Asset.h"
#include "StrikeEngine/Asset/Types/Template.h"
#include "StrikeEngine/Asset/ModelParser.h"
#include "StrikeEngine/Core/Log.h"
#include "StrikeEngine/Asset/AssetManager.h"

#include <string>
#include <filesystem>
#include <memory>
#include <future>
#include <unordered_map>
#include <mutex>
#include <pugixml.hpp>



namespace StrikeEngine {

    class AssetLoader {
    public:
        AssetLoader(const std::string& typeName);
        virtual ~AssetLoader() = default;

        const std::string& getTypeName() const;

        // Main load methods - now call loadAssetInternal
        std::shared_ptr<Asset> load(const std::string& id, const std::filesystem::path& path);
        
        // Async loading management
        void loadAsync(const std::string& id, const std::filesystem::path& path, std::shared_ptr<Asset> placeholderAsset);
        void update();
        void clearLoadingTasks();
        bool hasLoadingTasks() const; 

        virtual std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) = 0;
       
    protected:
        friend class Asset;

        // User implements this method to load asset data
        virtual std::shared_ptr<Asset> loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async=false) = 0;
        // Data swapping for async loading
        virtual void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) = 0;

        // Path resolution utilities
        std::filesystem::path resolvePath(const std::filesystem::path& src, const std::filesystem::path& basePath) const;
        std::filesystem::path addRootPrefix(const std::filesystem::path& path);

        // XML loading
        template<typename AssetType>
        std::shared_ptr<AssetType> loadFromNodeInternal(const pugi::xml_node& node, const std::filesystem::path& basePath);

    private:
        std::string mTypeName;

        struct LoadingTask {
            std::string id;
            std::filesystem::path path;
            std::shared_ptr<Asset> placeholderAsset;
            std::future<std::shared_ptr<Asset>> future;

            LoadingTask() = default;

            LoadingTask(const std::string& id, const std::filesystem::path& path,
                       std::shared_ptr<Asset> placeholder, std::future<std::shared_ptr<Asset>>&& fut)
                : id(id), path(path), placeholderAsset(placeholder), future(std::move(fut)) {}
        };

        std::unordered_map<std::string, LoadingTask> mLoadingTasks;
        mutable std::mutex mMutex;
    };


    template<typename AssetType>
    std::shared_ptr<AssetType> AssetLoader::loadFromNodeInternal(const pugi::xml_node& node, const std::filesystem::path& basePath){
        static_assert(std::is_base_of_v<Asset, AssetType>, "AssetType must derive from Asset");

        std::string id  = node.attribute("id").as_string();
        std::string src = node.attribute("src").as_string();
        bool async      = node.attribute("async").as_bool(false);

        if (id.empty() || src.empty()){
            STRIKE_CORE_ERROR("Invalid asset node: missing id or src attribute");
            
            auto asset = std::make_shared<AssetType>(id, src);
            asset->setLoadingState(AssetState::Failed);
            return asset;
        }

        auto asset = std::make_shared<AssetType>(id, src);
        asset->setLoadAsync(async);

        std::filesystem::path fullPath = resolvePath(src, basePath);

        if constexpr (std::is_same_v<AssetType, Template>)
        {
            ModelParser parser;
            if (!parser.parseModel(fullPath))
            {
                if (!std::filesystem::exists(fullPath))
                {
                    STRIKE_CORE_ERROR("Template model file does not exist: {}", fullPath.string());
                    asset->setLoadingState(AssetState::Failed);
                    return asset;
                }
            }
            fullPath.replace_extension(".tmpl");
        }

        if (async)
            return AssetManager::get().loadAsync<AssetType>(id, fullPath);
        else
            return AssetManager::get().load<AssetType>(id, fullPath);
        
    }

}
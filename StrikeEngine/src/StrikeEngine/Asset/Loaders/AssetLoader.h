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
        /**
         * @brief Construct a new Asset Loader object.
         * @param typeName The type name of the assets this loader handles same as asset type.
         */
        AssetLoader(const std::string& typeName);
        virtual ~AssetLoader() = default;

        /**
         * @brief Get the type name of the assets this loader handles.
         * @return Type name string.
         */
        const std::string& getTypeName() const;

        
        /**
         * @brief Load an asset from a file path.
         * @param id The unique identifier for the asset.
         * @param path The file path to the asset.
         * @return Shared pointer to the loaded asset.
         */
        std::shared_ptr<Asset> load(const std::string& id, const std::filesystem::path& path);
        
        /**
         * @brief Load an asset asynchronously.
         * @param id The unique identifier for the asset.
         *  @param path The file path to the asset.
         * @param placeholderAsset The placeholder asset to populate once loading is complete.
         */
        void loadAsync(const std::string& id, const std::filesystem::path& path, std::shared_ptr<Asset> placeholderAsset);

        /**
         * @brief Update the loader to process any completed asynchronous loading tasks.
         * @note This is called periodically in the main thread to finalize async loads.
         */
        void update();

        /**
         * @brief Clear all ongoing loading tasks.
         */
        void clearLoadingTasks();

        /**
         * @brief Check if there are any ongoing loading tasks.
         * @return True if there are loading tasks, false otherwise.
         */
        bool hasLoadingTasks() const; 

        /**
         * @brief Load an asset from an XML node.
         * @param node The XML node containing asset data.
         * @param basePath The base path to resolve relative asset paths.
         * @return Shared pointer to the loaded asset.
         */
        virtual std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) = 0;
       
    protected:
        friend class Asset;

        // User implements this method to load asset data
        virtual std::shared_ptr<Asset> loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async=false) = 0;
        // Data swapping for async loading
        virtual void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) = 0;

        // Path resolution utilities
        std::filesystem::path resolvePath(const std::filesystem::path& src, const std::filesystem::path& basePath) const;

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
    std::shared_ptr<AssetType> AssetLoader::loadFromNodeInternal(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        static_assert(std::is_base_of_v<Asset, AssetType>, "AssetType must derive from Asset");

        std::string id  = node.attribute("id").as_string();
        std::string src = node.attribute("src").as_string();

        if (id.empty() || src.empty()) {
            STRIKE_CORE_ERROR("Invalid asset node: missing id or src attribute");

            auto asset = std::make_shared<AssetType>(id, src);
            asset->setState(AssetState::Failed);
            return asset;
        }

        auto asset = std::make_shared<AssetType>(id, src);

        std::filesystem::path fullPath = resolvePath(src, basePath);
        
        // always load synchronously
        return AssetManager::get().load<AssetType>(id, fullPath);
    }


}
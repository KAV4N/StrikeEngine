#pragma once

#include <string>
#include <filesystem>
#include <memory>
#include <future>
#include <unordered_map>
#include <mutex>
#include <pugixml.hpp>

namespace StrikeEngine {

    class Asset;

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

        // XML loading
        virtual std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) = 0;

    protected:
        // User implements this method to load asset data
        virtual std::shared_ptr<Asset> loadAssetInternal(const std::string& id, const std::filesystem::path& path, bool async=false) = 0;

        // Placeholder creation
        virtual std::shared_ptr<Asset> createPlaceholder(const std::string& id, const std::filesystem::path& path) = 0;

        // Data swapping for async loading
        virtual void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) = 0;

        // Path resolution utilities
        std::filesystem::path resolvePath(const std::filesystem::path& src, const std::filesystem::path& basePath) const;
        std::filesystem::path addRootPrefix(const std::filesystem::path& path);

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
        std::mutex mMutex;
    };

}
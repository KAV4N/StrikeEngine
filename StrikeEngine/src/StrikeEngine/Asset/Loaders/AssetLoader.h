#pragma once
#include "StrikeEngine/Asset/Types/Asset.h"
#include <filesystem>
#include <memory>
#include <string>
#include <future>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>
#include <pugixml.hpp>

namespace StrikeEngine {
    class AssetLoader {
    public:
        AssetLoader(const std::string& typeName);
        virtual ~AssetLoader() = default;
        virtual std::shared_ptr<Asset> load(const std::string& id, const std::filesystem::path& path, bool async = false);
        virtual std::shared_ptr<Asset> loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) = 0;
        virtual void loadAsync(const std::string& id, const std::filesystem::path& path, std::shared_ptr<Asset> placeholderAsset);
        void update();
        void clearLoadingTasks();
        const std::string& getTypeName() const;

        std::filesystem::path addRootPrefix(const std::filesystem::path& path);

    protected:
        friend class AssetManager;
        virtual std::shared_ptr<Asset> createPlaceholder(const std::string& id, const std::filesystem::path& path) = 0;
        virtual void swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) = 0;

        // Resolve path with @ alias support
        std::filesystem::path resolvePath(const std::filesystem::path& src, const std::filesystem::path& basePath) const;

    protected:
        struct LoadingTask {
            std::string id;
            std::filesystem::path path;
            std::shared_ptr<Asset> placeholderAsset;
            std::future<std::shared_ptr<Asset>> future;
            bool flagOnlyPostLoad = false;
            LoadingTask()
                : id(),
                path(),
                placeholderAsset(nullptr),
                future(),
                flagOnlyPostLoad(false) {
            }
            LoadingTask(std::string id_,
                std::filesystem::path path_,
                std::shared_ptr<Asset> placeholderAsset_,
                std::future<std::shared_ptr<Asset>> future_,
                bool flagOnlyPostLoad_ = false)
                : id(std::move(id_)),
                path(std::move(path_)),
                placeholderAsset(std::move(placeholderAsset_)),
                future(std::move(future_)),
                flagOnlyPostLoad(flagOnlyPostLoad_) {
            }
        };
        std::unordered_map<std::string, LoadingTask> mLoadingTasks;
        std::string mTypeName;
        mutable std::mutex mMutex;
    };
}
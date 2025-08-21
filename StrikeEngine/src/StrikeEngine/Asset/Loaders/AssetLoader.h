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

namespace StrikeEngine {

    class AssetLoader {
    public:
        AssetLoader(const std::string& typeName);
        virtual ~AssetLoader() = default;

        virtual std::shared_ptr<Asset> load(const std::string& id, const std::filesystem::path& path) = 0;

        virtual void loadAsync(const std::string& id, const std::filesystem::path& path, std::shared_ptr<Asset> placeholderAsset);

        void update();

        void clearLoadingTasks();

        const std::string& getTypeName() const;

    protected:
        friend class AssetManager;
        virtual std::shared_ptr<Asset> createPlaceholder(const std::string& id, const std::filesystem::path& path) = 0;
        virtual bool swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loadedAsset);

    protected:
        struct LoadingTask {
            std::string id;
            std::filesystem::path path;
            std::shared_ptr<Asset> placeholderAsset;
            std::future<std::shared_ptr<Asset>> future;

            LoadingTask() = default;
            LoadingTask(const std::string& taskId, const std::filesystem::path& taskPath,
                std::shared_ptr<Asset> placeholder, std::future<std::shared_ptr<Asset>> taskFuture)
                : id(taskId), path(taskPath), placeholderAsset(std::move(placeholder)), future(std::move(taskFuture)) {
            }

            LoadingTask(LoadingTask&& other) noexcept
                : id(std::move(other.id)), path(std::move(other.path)),
                placeholderAsset(std::move(other.placeholderAsset)), future(std::move(other.future)) {
            }

            LoadingTask& operator=(LoadingTask&& other) noexcept {
                if (this != &other) {
                    id = std::move(other.id);
                    path = std::move(other.path);
                    placeholderAsset = std::move(other.placeholderAsset);
                    future = std::move(other.future);
                }
                return *this;
            }

            LoadingTask(const LoadingTask&) = delete;
            LoadingTask& operator=(const LoadingTask&) = delete;
        };

        std::unordered_map<std::string, LoadingTask> mLoadingTasks;
        std::string mTypeName;
        mutable std::mutex mMutex;
    };

}
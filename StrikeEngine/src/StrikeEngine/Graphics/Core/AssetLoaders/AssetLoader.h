#pragma once
#include "StrikeEngine/Graphics/Core/Assets/Asset.h"
#include <filesystem>
#include <future>
#include <memory>
#include <unordered_map>
#include <mutex>

namespace StrikeEngine {

    class AssetLoader {
    public:
        AssetLoader(AssetType type);
        virtual ~AssetLoader() = default;

        virtual std::shared_ptr<Asset> Load(const std::string& id, const std::filesystem::path& path) = 0;
        virtual std::shared_ptr<Asset> LoadTask(const std::string& id, const std::filesystem::path& path) = 0;

        AssetType GetType() const;

        // Queue an async load task
        void QueueLoadAsync(const std::string& id, const std::filesystem::path& path);

        // Process tasks and return completed assets
        std::vector<std::pair<std::string, std::shared_ptr<Asset>>> GetCompletedAssets();

        // Get IDs of assets currently loading
        std::vector<std::string> GetLoadingAssetIds() const;

        // Get count of assets currently loading
        size_t GetLoadingAssetCount() const;

    protected:
        struct LoadingTask {
            std::string id;
            std::filesystem::path filePath;
            std::future<std::shared_ptr<Asset>> future;
        };

        AssetType m_assetType;
        std::unordered_map<std::string, LoadingTask> m_loadingTasks;
        mutable std::mutex m_tasksMutex;
    };
}
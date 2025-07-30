#include "AssetLoader.h"
#include <iostream>

namespace StrikeEngine {

    AssetLoader::AssetLoader(AssetType type) : m_assetType(type) {}

    AssetType AssetLoader::GetType() const {
        return m_assetType;
    }

    void AssetLoader::QueueLoadAsync(const std::string& id, const std::filesystem::path& path) {
        std::lock_guard<std::mutex> lock(m_tasksMutex);
        if (m_loadingTasks.find(id) != m_loadingTasks.end()) {
            std::cerr << "Asset with ID '" << id << "' is already in loading tasks" << std::endl;
            return;
        }

        LoadingTask task;
        task.id = id;
        task.filePath = path;
        task.future = std::async(std::launch::async, [this, id, path]() {
            return this->LoadTask(id, path);
            });
        m_loadingTasks[id] = std::move(task);
    }

    std::vector<std::pair<std::string, std::shared_ptr<Asset>>> AssetLoader::GetCompletedAssets() {
        std::lock_guard<std::mutex> lock(m_tasksMutex);
        std::vector<std::pair<std::string, std::shared_ptr<Asset>>> completedAssets;

        for (auto it = m_loadingTasks.begin(); it != m_loadingTasks.end(); ) {
            auto& task = it->second;
            if (task.future.valid() &&
                task.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                try {
                    auto asset = task.future.get();
                    if (asset) {
                        completedAssets.emplace_back(task.id, asset);
                    }
                    else {
                        std::cerr << "Async loading failed for asset: " << task.id << std::endl;
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "Async asset loading failed for '" << task.id << "': " << e.what() << std::endl;
                }
                it = m_loadingTasks.erase(it);
            }
            else {
                ++it;
            }
        }
        return completedAssets;
    }

    std::vector<std::string> AssetLoader::GetLoadingAssetIds() const {
        std::lock_guard<std::mutex> lock(m_tasksMutex);
        std::vector<std::string> ids;
        ids.reserve(m_loadingTasks.size());
        for (const auto& [id, task] : m_loadingTasks) {
            ids.push_back(id);
        }
        return ids;
    }

    size_t AssetLoader::GetLoadingAssetCount() const {
        std::lock_guard<std::mutex> lock(m_tasksMutex);
        return m_loadingTasks.size();
    }
}
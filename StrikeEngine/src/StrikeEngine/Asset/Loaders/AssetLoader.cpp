#include "AssetLoader.h"
#include <algorithm>
#include "StrikeEngine/Asset/AssetManager.h"

#include "StrikeEngine/Asset/Types/Mesh.h";

namespace StrikeEngine {

    AssetLoader::AssetLoader(const std::string& typeName) : mTypeName(typeName) {}

    const std::string& AssetLoader::getTypeName() const {
        return mTypeName;
    }



    std::filesystem::path AssetLoader::resolvePath(const std::filesystem::path& src, const std::filesystem::path& basePath) const {
        std::string srcString = src.string();

        if (!srcString.empty() && srcString[0] == '@') {
            std::string pathWithoutAt = srcString.substr(1);

            if (!pathWithoutAt.empty() && (pathWithoutAt[0] == '/' || pathWithoutAt[0] == '\\')) {
                pathWithoutAt = pathWithoutAt.substr(1);
            }
            return pathWithoutAt;
        }
        else {
            return basePath / src;
        }
    }

    std::filesystem::path AssetLoader::addRootPrefix(const std::filesystem::path& path) {
        std::string pathStr = path.generic_string();
        std::filesystem::path newPath = path;

        if (!pathStr.empty() && (pathStr[0] == '/' || pathStr[0] == '\\')) {
            newPath = pathStr.substr(1);
        };

        return "@" / newPath;
    }

    std::shared_ptr<Asset> AssetLoader::load(const std::string& id, const std::filesystem::path& path, bool async) {
        return nullptr;
    }

    void AssetLoader::loadAsync(const std::string& id, const std::filesystem::path& path, std::shared_ptr<Asset> placeholderAsset) {
        std::lock_guard<std::mutex> lock(mMutex);
        if (mLoadingTasks.find(id) != mLoadingTasks.end()) {
            return;
        }
        placeholderAsset->setLoadAsync(true);
        auto future = std::async(std::launch::async, [this, id, path]() {
            return this->load(id, path, true);
            });
        mLoadingTasks.emplace(id, LoadingTask(id, path, placeholderAsset, std::move(future)));
    }

    void AssetLoader::update() {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mLoadingTasks.begin();
        while (it != mLoadingTasks.end()) {
            auto& task = it->second;
            if (task.flagOnlyPostLoad && task.placeholderAsset->isLoaded()) {
                task.placeholderAsset->postLoad();
                task.placeholderAsset->setLoadingState(AssetLoadingState::Ready);
                it = mLoadingTasks.erase(it);
            }
            else if (task.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                try {
                    auto loadedAsset = task.future.get();
                    if (loadedAsset && task.placeholderAsset) {
                        swapData(task.placeholderAsset, loadedAsset);
                        auto placeholderAsset = std::dynamic_pointer_cast<Mesh>(task.placeholderAsset);
                        task.placeholderAsset->postLoad();
                        task.placeholderAsset->setLoadingState(AssetLoadingState::Ready);
                    }
                    else if (task.placeholderAsset) {
                        // Loading failed
                        task.placeholderAsset->setLoadingState(AssetLoadingState::FAILED);
                    }
                }
                catch (const std::exception& e) {
                    if (task.placeholderAsset) {
                        task.placeholderAsset->setLoadingState(AssetLoadingState::FAILED);
                    }
                }
                it = mLoadingTasks.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void AssetLoader::clearLoadingTasks() {
        std::lock_guard<std::mutex> lock(mMutex);
        for (auto& [id, task] : mLoadingTasks) {
            if (task.placeholderAsset) {
                task.placeholderAsset->setLoadingState(AssetLoadingState::FAILED);
            }
        }
        mLoadingTasks.clear();
    }
}
#pragma once
#include "Scene.h"
#include "SceneLoader.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <future>

namespace StrikeEngine {
    enum class SceneLoadState {
        UNLOADED,
        LOADING,
        LOADED,
        FAILED
    };

    class SceneManager {
    public:
        static SceneManager& Get() {
            static SceneManager instance;
            return instance;
        }

        void SetScene(const std::string& scenePath);
        void LoadScene(const std::string& scenePath, const std::string& sceneId = "");

        void SetSceneAsync(const std::string& scenePath);
        void LoadSceneAsync(const std::string& scenePath, const std::string& sceneId = "");

        std::shared_ptr<Scene> GetCurrentScene() const { return m_currentScene; }
        std::shared_ptr<Scene> GetScene(const std::string& sceneId) const;
        void UnloadScene(const std::string& sceneId);
        void UnloadAllScenes();

        SceneLoadState GetSceneLoadState(const std::string& sceneId) const;
        bool IsSceneLoaded(const std::string& sceneId) const;
        bool IsSceneLoading(const std::string& sceneId) const;

        void Update(float deltaTime);

    private:
        SceneManager();
        ~SceneManager() = default;
        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;

        struct SceneData {
            std::shared_ptr<Scene> scene;
            SceneLoadState state = SceneLoadState::UNLOADED;
            std::future<std::shared_ptr<Scene>> loadFuture;
            std::string filePath;
        };

        std::unique_ptr<SceneLoader> m_sceneLoader;
        std::shared_ptr<Scene> m_currentScene;
        std::unordered_map<std::string, SceneData> m_scenes;

        void CheckAsyncLoads();
    };
}
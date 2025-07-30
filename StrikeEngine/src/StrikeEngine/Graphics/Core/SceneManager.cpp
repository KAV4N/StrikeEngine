#include "SceneManager.h"
#include <algorithm>

namespace StrikeEngine {

    SceneManager::SceneManager() {
        m_sceneLoader = std::make_unique<SceneLoader>();
    }

    void SceneManager::SetScene(const std::string& scenePath) {
        auto scene = m_sceneLoader->LoadScene(scenePath);
        if (scene) {
            m_currentScene = scene;

            std::string sceneId = scene->GetSceneId();
            m_scenes[sceneId].scene = scene;
            m_scenes[sceneId].state = SceneLoadState::LOADED;
            m_scenes[sceneId].filePath = scenePath;
        }
    }

    void SceneManager::LoadScene(const std::string& scenePath, const std::string& sceneId) {
        auto scene = m_sceneLoader->LoadScene(scenePath);
        if (scene) {
            std::string id = sceneId.empty() ? scene->GetSceneId() : sceneId;
            m_scenes[id].scene = scene;
            m_scenes[id].state = SceneLoadState::LOADED;
            m_scenes[id].filePath = scenePath;
        }
    }

    void SceneManager::SetSceneAsync(const std::string& scenePath) {
        std::string tempId = scenePath;
        auto& sceneData = m_scenes[tempId];
        sceneData.state = SceneLoadState::LOADING;
        sceneData.filePath = scenePath;
        sceneData.loadFuture = m_sceneLoader->LoadSceneAsync(scenePath);
    }

    void SceneManager::LoadSceneAsync(const std::string& scenePath, const std::string& sceneId) {
        std::string id = sceneId.empty() ? scenePath : sceneId;
        auto& sceneData = m_scenes[id];
        sceneData.state = SceneLoadState::LOADING;
        sceneData.filePath = scenePath;
        sceneData.loadFuture = m_sceneLoader->LoadSceneAsync(scenePath);
    }

    std::shared_ptr<Scene> SceneManager::GetScene(const std::string& sceneId) const {
        auto it = m_scenes.find(sceneId);
        return (it != m_scenes.end() && it->second.state == SceneLoadState::LOADED)
            ? it->second.scene : nullptr;
    }

    void SceneManager::UnloadScene(const std::string& sceneId) {
        auto it = m_scenes.find(sceneId);
        if (it != m_scenes.end()) {
            if (m_currentScene && m_currentScene->GetSceneId() == sceneId) {
                m_currentScene.reset();
            }
            m_scenes.erase(it);
        }
    }

    void SceneManager::UnloadAllScenes() {
        m_currentScene.reset();
        m_scenes.clear();
    }

    SceneLoadState SceneManager::GetSceneLoadState(const std::string& sceneId) const {
        auto it = m_scenes.find(sceneId);
        return (it != m_scenes.end()) ? it->second.state : SceneLoadState::UNLOADED;
    }

    bool SceneManager::IsSceneLoaded(const std::string& sceneId) const {
        return GetSceneLoadState(sceneId) == SceneLoadState::LOADED;
    }

    bool SceneManager::IsSceneLoading(const std::string& sceneId) const {
        return GetSceneLoadState(sceneId) == SceneLoadState::LOADING;
    }

    void SceneManager::Update(float deltaTime) {
        CheckAsyncLoads();

        if (m_currentScene) {
            m_currentScene->Update(deltaTime);
        }
    }

    void SceneManager::CheckAsyncLoads() {
        for (auto& [sceneId, sceneData] : m_scenes) {
            if (sceneData.state == SceneLoadState::LOADING && sceneData.loadFuture.valid()) {
                if (sceneData.loadFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    try {
                        auto scene = sceneData.loadFuture.get();
                        if (scene) {
                            sceneData.scene = scene;
                            sceneData.state = SceneLoadState::LOADED;

                            if (sceneId == sceneData.filePath) {
                                m_currentScene = scene;
                                std::string actualId = scene->GetSceneId();
                                if (actualId != sceneId) {
                                    m_scenes[actualId] = std::move(sceneData);
                                    m_scenes.erase(sceneId);
                                }
                            }
                        }
                        else {
                            sceneData.state = SceneLoadState::FAILED;
                        }
                    }
                    catch (const std::exception&) {
                        sceneData.state = SceneLoadState::FAILED;
                    }
                }
            }
        }
    }

} // namespace StrikeEngine
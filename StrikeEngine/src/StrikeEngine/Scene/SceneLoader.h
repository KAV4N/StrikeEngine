#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <future>
#include <vector>
#include <unordered_set>
#include <pugixml.hpp>
#include "glm/glm.hpp"

namespace StrikeEngine {
    class Scene;
    class Entity;

    class SceneLoader {
    public:
        SceneLoader();
        ~SceneLoader() = default;

        std::future<std::unique_ptr<Scene>> loadScene(const std::filesystem::path& path);
        std::future<std::unique_ptr<Scene>> loadSceneAsync(const std::filesystem::path& path);
        
        bool isLoading() const;
        void update();

    private:
        std::unique_ptr<Scene> loadSceneInternal(const std::filesystem::path& path, bool async = false);
        
        void loadAssets(const pugi::xml_node& assetsNode, const std::filesystem::path& basePath, bool async = false);
        
        void createEntities(Scene& scene, const pugi::xml_node& entitiesNode, Entity parentEntity);
        
        void setupSkybox(Scene& scene, const pugi::xml_node& sceneNode);
        void setupSun(Scene& scene, const pugi::xml_node& sceneNode);
        
        glm::vec3 parseVector3(const std::string& str);

    private:
        bool mIsLoading;
    };
}
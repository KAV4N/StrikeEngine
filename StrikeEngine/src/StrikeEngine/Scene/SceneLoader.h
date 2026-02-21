#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <vector>
#include <unordered_set>
#include <pugixml.hpp>
#include "glm/glm.hpp"

namespace Strike {
    class Scene;
    class Entity;

    /**
     * @brief Loader for scenes from XML files.
     */
    class SceneLoader {
    public:
        SceneLoader();
        ~SceneLoader() = default;
        /**
         * @brief Load a scene from the specified file path.
         * @param path The file path to the scene file.
         * @return Unique pointer to the loaded Scene object.
         * @note Returns nullptr if loading failed.
         */
        std::unique_ptr<Scene> loadScene(const std::filesystem::path& path);

        
        bool isLoading() const { return mIsLoading;}

    private:
        
        void loadAssets(const pugi::xml_node& assetsNode, const std::filesystem::path& basePath);
        
        void createEntities(Scene& scene, const pugi::xml_node& entitiesNode, Entity parentEntity);
        
        void setupSkybox(Scene& scene, const pugi::xml_node& sceneNode);
        void setupSun(Scene& scene, const pugi::xml_node& sceneNode);
        void setupFog(const pugi::xml_node& sceneNode);  
        
        glm::vec3 parseVector3(const std::string& str);

    private:
        bool mIsLoading = false;
    };
}
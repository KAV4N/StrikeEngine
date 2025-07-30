#pragma once
#include "Scene.h"
#include <pugixml.hpp>
#include <string>
#include <memory>
#include <future>

namespace StrikeEngine {
    class SceneLoader {
    public:
        SceneLoader();
        ~SceneLoader() = default;

        std::shared_ptr<Scene> LoadScene(const std::string& filePath);
        std::future<std::shared_ptr<Scene>> LoadSceneAsync(const std::string& filePath);

    private:
        void LoadAssets(const pugi::xml_node& assetsNode);
        std::shared_ptr<Entity> LoadEntity(const pugi::xml_node& entityNode, Scene& scene, entt::entity parentId = entt::null);
        void LoadComponents(const pugi::xml_node& componentsNode, Entity& entity);

        glm::vec3 ParseVector3(const std::string& str) const;
        std::shared_ptr<Scene> LoadSceneInternal(const std::string& filePath);
    };
}
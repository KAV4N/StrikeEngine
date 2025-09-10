#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include <future>
#include <pugixml.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Scene.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "ComponentRegistry.h"
#include "StrikeEngine/Asset/ModelParser.h"

namespace StrikeEngine {
    class Scene;
    class Entity;
    class ComponentRegistry;
    class AssetManager;

    class SceneLoader final {
    public:
        SceneLoader();
        ~SceneLoader() = default;

        // Synchronous loading
        std::unique_ptr<Scene> loadScene(const std::filesystem::path& filePath);

        // Asynchronous loading
        std::future<std::unique_ptr<Scene>> loadSceneAsync(const std::filesystem::path& filePath);

        // Check if a scene is currently loading
        bool isLoading() const;

    private:
        // Asset loading
        void loadAssetsFromXML(const pugi::xml_node& assetsNode, bool async);

        // Entity parsing
        void parseEntity(Scene& scene, const pugi::xml_node& entityNode, Entity parent = Entity(), const std::string& idPrefix = "");

        // Component parsing
        void parseEntityComponents(Entity& entity, const pugi::xml_node& componentsNode);

        // Template handling
        void instantiateTemplate(Scene& scene, const pugi::xml_node& entityNode, Entity parent, const std::string& idPrefix);

        // Utility functions
        glm::vec3 parseVec3(const std::string& str, const glm::vec3& defaultValue = glm::vec3(0.0f));
        glm::quat parseRotation(const std::string& str);
        std::unordered_map<std::string, std::string> parseAttributes(const pugi::xml_node& node);

        // Validation
        bool validateSceneFile(const pugi::xml_document& doc);

        // Internal async loading implementation
        std::unique_ptr<Scene> loadSceneInternal(const std::filesystem::path& filePath);

    private:
        AssetManager& mAssetManager;
        ComponentRegistry& mComponentRegistry;
        ModelParser mModelParser;
        std::atomic<bool> mIsLoading{ false };
    };
}
#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <filesystem>
#include <future>
#include <pugixml.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
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


        std::unique_ptr<Scene> loadScene(const std::filesystem::path& filePath);

        std::future<std::unique_ptr<Scene>> loadSceneAsync(const std::filesystem::path& filePath);

        bool isLoading() const;

        void update();

    private:
        void loadAssetsFromXML(const pugi::xml_node& assetsNode, Scene* scene, const std::filesystem::path& basePath);

        void parseEntity(Scene& scene, const pugi::xml_node& entityNode, Entity parent = Entity(), const std::string& idPrefix = "");

        void parseEntityComponents(Entity& entity, const pugi::xml_node& componentsNode);

        void instantiateTemplate(Scene& scene, const pugi::xml_node& entityNode, Entity parent, const std::string& idPrefix);

        glm::vec3 parseVec3(const std::string& str, const glm::vec3& defaultValue = glm::vec3(0.0f));
        glm::quat parseRotation(const std::string& str);
        std::unordered_map<std::string, std::string> parseAttributes(const pugi::xml_node& node);

        bool validateSceneFile(const pugi::xml_document& doc);

        std::unique_ptr<Scene> loadSceneInternal(const std::filesystem::path& filePath);

        struct PendingTemplateInstantiation {
            std::string templateId;
            std::string id;
            std::string name;
            std::string idPrefix;
            glm::vec3 position;
            glm::quat rotation;
            glm::vec3 scale;
            Entity parent;
            Scene* scene;
            pugi::xml_node entityNode;
        };

        void processPendingTemplates();

        void addPendingTemplateInstantiation(const std::string& templateId, const std::string& id, const std::string& name,
            const std::string& idPrefix, const glm::vec3& position, const glm::quat& rotation,
            const glm::vec3& scale, Entity parent, Scene* scene, const pugi::xml_node& entityNode);

    private:
        AssetManager& mAssetManager;
        ComponentRegistry& mComponentRegistry;
        ModelParser mModelParser;
        std::atomic<bool> mIsLoading{ false };
        std::vector<PendingTemplateInstantiation> mPendingTemplates;
        mutable std::mutex mPendingTemplatesMutex;
    };
}
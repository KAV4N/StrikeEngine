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
        // Core entity data structure
        struct EntityData {
            std::string id;
            std::string name;
            std::string templateId;
            glm::vec3 position{ 0.0f };
            glm::vec3 rotation{ 0.0f };
            glm::vec3 scale{ 1.0f };
            pugi::xml_node componentsNode;
            pugi::xml_node childrenNode;

            bool isTemplate() const { return !templateId.empty(); }
            std::string getDisplayName() const { return name.empty() ? id : name; }
        };

        struct PendingTemplate {
            EntityData entityData;
            std::string prefixedId;
            std::string idPrefix;
            Entity parent;
            Scene* scene;
        };

        // Main loading functions
        std::unique_ptr<Scene> loadSceneInternal(const std::filesystem::path& filePath);
        void loadAssetsFromXML(const pugi::xml_node& assetsNode, Scene* scene, const std::filesystem::path& basePath);

        // Entity parsing and creation
        EntityData parseEntityData(const pugi::xml_node& entityNode);
        void processEntity(Scene& scene, const EntityData& data, Entity parent, const std::string& idPrefix);
        Entity createEntityFromData(Scene& scene, const EntityData& data, const std::string& prefixedId);
        void applyEntityTransform(Entity& entity, const EntityData& data);
        void parseEntityComponents(Entity& entity, const pugi::xml_node& componentsNode);
        void processEntityChildren(Scene& scene, const EntityData& data, Entity parent, const std::string& prefixedId);

        // Template handling
        void handleTemplateEntity(Scene& scene, const EntityData& data, Entity parent, const std::string& idPrefix);
        void instantiateReadyTemplate(Scene& scene, const EntityData& data, const std::string& prefixedId, Entity parent);
        void queuePendingTemplate(const EntityData& data, const std::string& prefixedId, const std::string& idPrefix, Entity parent, Scene* scene);
        void processPendingTemplates();

        // Utility functions
        std::string buildPrefixedId(const std::string& id, const std::string& idPrefix);
        glm::vec3 parseVec3(const std::string& str, const glm::vec3& defaultValue = glm::vec3(0.0f));
        glm::quat parseRotation(const std::string& str);
        bool validateSceneFile(const pugi::xml_document& doc);
        void logEntityCreation(const std::string& prefixedId, const std::string& name, bool isTemplate = false);
        void logTemplateStatus(const std::string& templateId, const std::string& prefixedId, bool instantiated);

    private:
        AssetManager& mAssetManager;
        ComponentRegistry& mComponentRegistry;
        ModelParser mModelParser;
        std::atomic<bool> mIsLoading{ false };
        std::vector<PendingTemplate> mPendingTemplates;
        mutable std::mutex mPendingTemplatesMutex;
    };
}
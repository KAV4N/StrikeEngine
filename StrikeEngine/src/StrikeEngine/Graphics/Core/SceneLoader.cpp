#include "SceneLoader.h"
#include "ComponentRegistry.h"
#include "AssetManager.h"
#include <filesystem>
#include <sstream>
#include <future>
#include <unordered_map>

namespace StrikeEngine {
    SceneLoader::SceneLoader() {}

    std::shared_ptr<Scene> SceneLoader::LoadScene(const std::string& filePath) {
        return LoadSceneInternal(filePath);
    }

    std::future<std::shared_ptr<Scene>> SceneLoader::LoadSceneAsync(const std::string& filePath) {
        return std::async(std::launch::async, [this, filePath]() {
            return LoadSceneInternal(filePath);
            });
    }

    std::shared_ptr<Scene> SceneLoader::LoadSceneInternal(const std::string& filePath) {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(filePath.c_str());

        if (!result) {
            return nullptr;
        }

        pugi::xml_node sceneNode = doc.child("scene");
        if (!sceneNode) {
            return nullptr;
        }

        std::string sceneId = sceneNode.attribute("sceneId").as_string();
        std::string sceneName = sceneNode.attribute("name").as_string();
        auto scene = std::make_shared<Scene>(sceneId, sceneName);

        pugi::xml_node assetsNode = sceneNode.child("assets");
        if (assetsNode) {
            LoadAssets(assetsNode);
        }

        pugi::xml_node entitiesNode = sceneNode.child("entities");
        if (entitiesNode) {
            for (pugi::xml_node entityNode : entitiesNode.children("entity")) {
                LoadEntity(entityNode, *scene);
            }
        }

        return scene;
    }

    void SceneLoader::LoadAssets(const pugi::xml_node& assetsNode) {
        auto& assetManager = AssetManager::Get();

        for (pugi::xml_node meshNode : assetsNode.children("mesh")) {
            std::string meshId = meshNode.attribute("meshId").as_string();
            std::string src = meshNode.attribute("src").as_string();
            assetManager.LoadAssetByType(AssetType::MESH, meshId, src);
        }

        for (pugi::xml_node materialNode : assetsNode.children("material")) {
            std::string materialId = materialNode.attribute("materialId").as_string();
            std::string src = materialNode.attribute("src").as_string();
            assetManager.LoadAssetByType(AssetType::MATERIAL, materialId, src);
        }

        for (pugi::xml_node textureNode : assetsNode.children("texture")) {
            std::string textureId = textureNode.attribute("textureId").as_string();
            std::string src = textureNode.attribute("src").as_string();
            assetManager.LoadAssetByType(AssetType::TEXTURE, textureId, src);
        }
    }

    std::shared_ptr<Entity> SceneLoader::LoadEntity(const pugi::xml_node& entityNode, Scene& scene, entt::entity parentId) {
        std::string id = entityNode.attribute("id").as_string();
        std::string name = entityNode.attribute("name").as_string();

        auto entity = scene.CreateEntity(name, id);

        std::string positionStr = entityNode.attribute("position").as_string();
        std::string rotationStr = entityNode.attribute("rotation").as_string();
        std::string scaleStr = entityNode.attribute("scale").as_string();

        if (!positionStr.empty()) {
            entity->SetPosition(ParseVector3(positionStr));
        }
        if (!rotationStr.empty()) {
            entity->SetRotation(ParseVector3(rotationStr));
        }
        if (!scaleStr.empty()) {
            entity->SetScale(ParseVector3(scaleStr));
        }

        if (parentId != entt::null) {
            entity->SetParent(parentId);
            auto& rootEntities = const_cast<std::vector<std::shared_ptr<Entity>>&>(scene.GetRootEntities());
            auto it = std::find(rootEntities.begin(), rootEntities.end(), entity);
            if (it != rootEntities.end()) {
                rootEntities.erase(it);
            }
        }

        pugi::xml_node componentsNode = entityNode.child("components");
        if (componentsNode) {
            LoadComponents(componentsNode, *entity);
        }

        pugi::xml_node childrenNode = entityNode.child("children");
        if (childrenNode) {
            for (pugi::xml_node childEntityNode : childrenNode.children("entity")) {
                LoadEntity(childEntityNode, scene, entity->GetHandle());
            }
        }

        return entity;
    }

    void SceneLoader::LoadComponents(const pugi::xml_node& componentsNode, Entity& entity) {
        auto& componentRegistry = ComponentRegistry::Get();

        for (pugi::xml_node componentNode : componentsNode.children()) {
            std::string componentType = componentNode.name();

            if (!componentRegistry.HasComponent(componentType)) {
                continue;
            }

            std::unordered_map<std::string, std::string> attributes;
            for (pugi::xml_attribute attr : componentNode.attributes()) {
                attributes[attr.name()] = attr.as_string();
            }

            componentRegistry.AddComponentToEntity(entity, componentType, attributes, componentNode);
        }
    }

    glm::vec3 SceneLoader::ParseVector3(const std::string& str) const {
        glm::vec3 result(0.0f);
        std::stringstream ss(str);
        std::string token;

        int i = 0;
        while (std::getline(ss, token, ',') && i < 3) {
            result[i] = std::stof(token);
            ++i;
        }

        return result;
    }
}
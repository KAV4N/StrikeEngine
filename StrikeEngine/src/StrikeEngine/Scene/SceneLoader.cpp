#include "SceneLoader.h"
#include "Entity.h"
#include "StrikeEngine/Asset/Types/Template.h"
#include <iostream>
#include <sstream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace StrikeEngine {

    SceneLoader::SceneLoader()
        : mAssetManager(AssetManager::get())
        , mComponentRegistry(ComponentRegistry::get())
    {
    }

    std::unique_ptr<Scene> SceneLoader::loadScene(const std::filesystem::path& filePath) {
        if (mIsLoading) {
            std::cerr << "Cannot load scene: another scene is currently being loaded" << std::endl;
            return nullptr;
        }
        mIsLoading = true;
        auto scene = loadSceneInternal(filePath);
        mIsLoading = false;

        processPendingTemplates();

        return scene;
    }

    std::future<std::unique_ptr<Scene>> SceneLoader::loadSceneAsync(const std::filesystem::path& filePath) {
        if (mIsLoading) {
            std::cerr << "Cannot load scene asynchronously: another scene is currently being loaded" << std::endl;
            return std::future<std::unique_ptr<Scene>>();
        }
        mIsLoading = true;

        return std::async(std::launch::async, [this, filePath]() {
            auto scene = loadSceneInternal(filePath);
            mIsLoading = false;
            return scene;
            });
    }

    bool SceneLoader::isLoading() const {
        return mIsLoading;
    }

    void SceneLoader::update() {
        processPendingTemplates();
    }

    void SceneLoader::processPendingTemplates() {
        std::lock_guard<std::mutex> lock(mPendingTemplatesMutex);
        auto it = mPendingTemplates.begin();
        while (it != mPendingTemplates.end()) {
            auto mesh = mAssetManager.getMesh("mesh789");
 
            auto templateAsset = mAssetManager.getTemplate(it->templateId);
            if (templateAsset && templateAsset->isReady()) {
                // Create parent entity for the template
                Entity templateParent = Entity::create(it->scene, it->id);
                templateParent.setName(it->name);
                templateParent.setPosition(it->position);
                templateParent.setEulerRotation(it->rotation);
                templateParent.setScale(it->scale);

                if (it->parent.isValid()) {
                    templateParent.setParent(it->parent);
                }

                // Instantiate the template
                templateAsset->instantiate(*it->scene, templateParent, it->idPrefix);

                std::cout << "Instantiated pending template: " << it->templateId << " as entity: " << it->id << " (" << it->name << ")" << std::endl;

                // Remove from pending list
                it = mPendingTemplates.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void SceneLoader::addPendingTemplateInstantiation(const std::string& templateId, const std::string& id, const std::string& name,
        const std::string& idPrefix, const glm::vec3& position, const glm::vec3& rotation,
        const glm::vec3& scale, Entity parent, Scene* scene, const pugi::xml_node& entityNode) {
        std::lock_guard<std::mutex> lock(mPendingTemplatesMutex);
        mPendingTemplates.emplace_back(PendingTemplateInstantiation{
            templateId, id, name, idPrefix, position, rotation, scale, parent, scene, entityNode
            });
    }

    std::unique_ptr<Scene> SceneLoader::loadSceneInternal(const std::filesystem::path& filePath) {
        if (!std::filesystem::exists(filePath)) {
            std::cerr << "Scene file does not exist: " << filePath << std::endl;
            return nullptr;
        }

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(filePath.string().c_str());

        if (!result) {
            std::cerr << "Failed to parse scene file: " << filePath << " - " << result.description() << std::endl;
            return nullptr;
        }

        if (!validateSceneFile(doc)) {
            std::cerr << "Invalid scene file format: " << filePath << std::endl;
            return nullptr;
        }

        pugi::xml_node sceneNode = doc.child("scene");

        // Extract scene information
        std::string sceneId = sceneNode.attribute("sceneId").as_string();
        if (sceneId.empty()) {
            std::cerr << "Scene must have a sceneId attribute" << std::endl;
            return nullptr;
        }

        // Create the scene
        auto scene = std::make_unique<Scene>(sceneId, filePath, sceneId);

        // Parse root entity transform
        Entity rootEntity = scene->getRootEntity();
        std::string position = sceneNode.attribute("position").as_string("0.0,0.0,0.0");
        std::string rotation = sceneNode.attribute("rotation").as_string("0.0,0.0,0.0");
        std::string scale = sceneNode.attribute("scale").as_string("1.0,1.0,1.0");

        rootEntity.setPosition(parseVec3(position));
        rootEntity.setEulerRotation(parseVec3(rotation));
        rootEntity.setScale(parseVec3(scale, glm::vec3(1.0f)));

        // Parse root entity components
        pugi::xml_node rootComponentsNode = sceneNode.child("components");
        if (rootComponentsNode) {
            parseEntityComponents(rootEntity, rootComponentsNode);
        }

        // Get the base path (directory of the scene file)
        std::filesystem::path basePath = filePath.parent_path();

        // Load assets first
        pugi::xml_node assetsNode = sceneNode.child("assets");
        if (assetsNode) {
            loadAssetsFromXML(assetsNode, scene.get(), basePath);
        }

        // Parse entities
        pugi::xml_node entitiesNode = sceneNode.child("entities");
        if (entitiesNode) {
            for (pugi::xml_node entityNode : entitiesNode.children("entity")) {
                parseEntity(*scene, entityNode, rootEntity);
            }
        }

        std::cout << "Successfully loaded scene: " << sceneId << " from " << filePath << std::endl;
        return scene;
    }

    void SceneLoader::loadAssetsFromXML(const pugi::xml_node& assetsNode, Scene* scene, const std::filesystem::path& basePath) {
        mAssetManager.deserialize(assetsNode, scene->getSceneAssets(), basePath);
    }

    void SceneLoader::parseEntity(Scene& scene, const pugi::xml_node& entityNode, Entity parent, const std::string& idPrefix) {
        std::string templateId = entityNode.attribute("templateId").as_string();
        if (!templateId.empty()) {
            // Handle template instantiation
            instantiateTemplate(scene, entityNode, parent, idPrefix);
            return;
        }

        // Extract entity attributes
        std::string id = entityNode.attribute("id").as_string();
        std::string name = entityNode.attribute("name").as_string();

        if (id.empty()) {
            std::cerr << "Entity missing id attribute" << std::endl;
            return;
        }

        // Apply idPrefix if provided
        std::string prefixedId = idPrefix.empty() ? id : idPrefix + "." + id;
        if (name.empty()) {
            name = id; // Use id as name if name is not provided
        }

        // Create entity
        Entity entity = Entity::create(&scene, prefixedId);
        entity.setName(name);

        // Set parent if provided
        if (parent.isValid()) {
            entity.setParent(parent);
        }

        // Parse transform
        std::string position = entityNode.attribute("position").as_string("0.0,0.0,0.0");
        std::string rotation = entityNode.attribute("rotation").as_string("0.0,0.0,0.0");
        std::string scale = entityNode.attribute("scale").as_string("1.0,1.0,1.0");

        entity.setPosition(parseVec3(position));
        entity.setEulerRotation(parseVec3(rotation));
        entity.setScale(parseVec3(scale, glm::vec3(1.0f)));

        // Parse components
        pugi::xml_node componentsNode = entityNode.child("components");
        if (componentsNode) {
            parseEntityComponents(entity, componentsNode);
        }

        // Parse children recursively
        pugi::xml_node childrenNode = entityNode.child("children");
        if (childrenNode) {
            for (pugi::xml_node childEntityNode : childrenNode.children("entity")) {
                parseEntity(scene, childEntityNode, entity, prefixedId);
            }
        }

        std::cout << "Created entity: " << prefixedId << " (" << name << ")" << std::endl;
    }

    void SceneLoader::instantiateTemplate(Scene& scene, const pugi::xml_node& entityNode, Entity parent, const std::string& idPrefix) {
        std::string templateId = entityNode.attribute("templateId").as_string();
        std::string id = entityNode.attribute("id").as_string();
        std::string name = entityNode.attribute("name").as_string();

        if (id.empty()) {
            std::cerr << "Template entity missing id attribute" << std::endl;
            return;
        }

        if (name.empty()) {
            name = id;
        }

        // Apply idPrefix if provided
        std::string prefixedId = idPrefix.empty() ? id : idPrefix + "." + id;

        // Parse transform
        std::string position = entityNode.attribute("position").as_string("0.0,0.0,0.0");
        std::string rotation = entityNode.attribute("rotation").as_string("0.0,0.0,0.0");
        std::string scale = entityNode.attribute("scale").as_string("1.0,1.0,1.0");

        glm::vec3 localPos = parseVec3(position);
        glm::vec3 localRot = parseVec3(rotation);
        glm::vec3 localScale = parseVec3(scale, glm::vec3(1.0f));

        // Check if template is ready
        auto templateAsset = mAssetManager.getTemplate(templateId);
        if (templateAsset && templateAsset->isReady()) {
            // Create parent entity for the template
            Entity templateParent = Entity::create(&scene, prefixedId);
            templateParent.setName(name);

            templateParent.setPosition(localPos);
            templateParent.setEulerRotation(localRot);
            templateParent.setScale(localScale);

            if (parent.isValid()) {
                templateParent.setParent(parent);
            }

            // Instantiate the template
            templateAsset->instantiate(scene, templateParent, prefixedId);

            std::cout << "Instantiated template: " << templateId << " as entity: " << prefixedId << " (" << name << ")" << std::endl;
        }
        else {
            addPendingTemplateInstantiation(templateId, prefixedId, name, idPrefix, localPos, localRot, localScale, parent, &scene, entityNode);
            std::cout << "Template " << templateId << " not ready, queued for instantiation as entity: " << prefixedId << std::endl;
        }
    }

    void SceneLoader::parseEntityComponents(Entity& entity, const pugi::xml_node& componentsNode) {
        for (pugi::xml_node componentNode : componentsNode.children()) {
            std::string componentType = componentNode.name();

            if (mComponentRegistry.isRegistered(componentType)) {
                mComponentRegistry.addComponentToEntity(entity, componentType, componentNode);
                std::cout << "Added component: " << componentType << " to entity: " << entity.getId() << std::endl;
            }
            else {
                std::cerr << "Unknown component type: " << componentType << std::endl;
            }
        }
    }

    glm::vec3 SceneLoader::parseVec3(const std::string& str, const glm::vec3& defaultValue) {
        if (str.empty()) {
            return defaultValue;
        }

        std::istringstream iss(str);
        std::string token;
        glm::vec3 result = defaultValue;
        int index = 0;

        while (std::getline(iss, token, ',') && index < 3) {
            try {
                result[index] = std::stof(token);
            }
            catch (const std::exception& e) {
                std::cerr << "Failed to parse vec3 component: " << token << std::endl;
                return defaultValue;
            }
            index++;
        }

        return result;
    }

    glm::quat SceneLoader::parseRotation(const std::string& str) {
        glm::vec3 eulerDegrees = parseVec3(str, glm::vec3(0.0f));
        glm::vec3 eulerRadians = glm::radians(eulerDegrees);
        return glm::quat(eulerRadians);
    }

    bool SceneLoader::validateSceneFile(const pugi::xml_document& doc) {
        pugi::xml_node sceneNode = doc.child("scene");
        if (!sceneNode) {
            std::cerr << "Missing root 'scene' node" << std::endl;
            return false;
        }

        if (sceneNode.attribute("sceneId").empty()) {
            std::cerr << "Scene node must have 'sceneId' attribute" << std::endl;
            return false;
        }

        return true;
    }
}
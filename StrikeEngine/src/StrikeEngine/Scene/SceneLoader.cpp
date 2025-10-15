#include "SceneLoader.h"
#include "Entity.h"
#include "StrikeEngine/Asset/Types/Template.h"
#include "World.h"
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

    // ========== Core Loading Functions ==========

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
        std::string sceneId = sceneNode.attribute("sceneId").as_string();
        if (sceneId.empty()) {
            std::cerr << "Scene must have a sceneId attribute" << std::endl;
            return nullptr;
        }

        // Create scene and setup root entity
        auto scene = std::make_unique<Scene>(sceneId, filePath, sceneId);
        Entity rootEntity = scene->getRootEntity();

        // Parse root entity data and apply it
        EntityData rootData;
        rootData.position = parseVec3(sceneNode.attribute("position").as_string("0.0,0.0,0.0"));
        rootData.rotation = parseVec3(sceneNode.attribute("rotation").as_string("0.0,0.0,0.0"));
        rootData.scale = parseVec3(sceneNode.attribute("scale").as_string("1.0,1.0,1.0"), glm::vec3(1.0f));
        rootData.componentsNode = sceneNode.child("components");

        applyEntityTransform(rootEntity, rootData);
        if (rootData.componentsNode) {
            parseEntityComponents(rootEntity, rootData.componentsNode);
        }

        // Load assets
        pugi::xml_node assetsNode = sceneNode.child("assets");
        if (assetsNode) {
            loadAssetsFromXML(assetsNode, scene.get(), filePath.parent_path());
        }

        // Load skybox
        pugi::xml_node skyboxNode = sceneNode.child("skybox");
        if (skyboxNode) {
            loadSkybox(skyboxNode);
        }

        // Process all entities
        pugi::xml_node entitiesNode = sceneNode.child("entities");
        if (entitiesNode) {
            for (pugi::xml_node entityNode : entitiesNode.children("entity")) {
                EntityData entityData = parseEntityData(entityNode);
                processEntity(*scene, entityData, rootEntity, "");
            }
        }

        std::cout << "Successfully loaded scene: " << sceneId << " from " << filePath << std::endl;
        return scene;
    }

    void SceneLoader::loadAssetsFromXML(const pugi::xml_node& assetsNode, Scene* scene, const std::filesystem::path& basePath) {
        mAssetManager.deserialize(assetsNode, scene->getSceneAssets(), basePath);
    }

    void SceneLoader::loadSkybox(const pugi::xml_node& skyboxNode) {
        std::string cubeMapId = skyboxNode.attribute("cubeMapId").as_string();
        std::string shaderId = skyboxNode.attribute("shaderId").as_string("");

        if (!cubeMapId.empty()) {
            auto& world = World::get();
            auto skybox = world.getSkybox();
            skybox->setCubeMap(cubeMapId);
            if (!shaderId.empty()) {
                skybox->setShader(shaderId);
            }
            std::cout << "Loaded skybox with cubeMapId: " << cubeMapId
                << (shaderId.empty() ? "" : " and shaderId: " + shaderId) << std::endl;
        }
        else {
            std::cerr << "Skybox node missing required cubeMapId attribute" << std::endl;
        }
    }

    // ========== Entity Data Parsing ==========

    SceneLoader::EntityData SceneLoader::parseEntityData(const pugi::xml_node& entityNode) {
        EntityData data;
        data.id = entityNode.attribute("id").as_string();
        data.name = entityNode.attribute("name").as_string();
        data.templateId = entityNode.attribute("templateId").as_string();
        data.position = parseVec3(entityNode.attribute("position").as_string("0.0,0.0,0.0"));
        data.rotation = parseVec3(entityNode.attribute("rotation").as_string("0.0,0.0,0.0"));
        data.scale = parseVec3(entityNode.attribute("scale").as_string("1.0,1.0,1.0"), glm::vec3(1.0f));
        data.componentsNode = entityNode.child("components");
        data.childrenNode = entityNode.child("children");
        return data;
    }

    void SceneLoader::processEntity(Scene& scene, const EntityData& data, Entity parent, const std::string& idPrefix) {
        if (data.id.empty()) {
            std::cerr << "Entity missing id attribute" << std::endl;
            return;
        }

        if (data.isTemplate()) {
            handleTemplateEntity(scene, data, parent, idPrefix);
        }
        else {
            std::string prefixedId = buildPrefixedId(data.id, idPrefix);
            Entity entity = createEntityFromData(scene, data, prefixedId);

            if (parent.isValid()) {
                entity.setParent(parent);
            }

            processEntityChildren(scene, data, entity, prefixedId);
            logEntityCreation(prefixedId, data.getDisplayName());
        }
    }

    Entity SceneLoader::createEntityFromData(Scene& scene, const EntityData& data, const std::string& prefixedId) {
        Entity entity = Entity::create(&scene, prefixedId);
        entity.setName(data.getDisplayName());

        applyEntityTransform(entity, data);

        if (data.componentsNode) {
            parseEntityComponents(entity, data.componentsNode);
        }

        return entity;
    }

    void SceneLoader::applyEntityTransform(Entity& entity, const EntityData& data) {
        if (entity.isValid()) {
            entity.setPosition(data.position);
            entity.setRotationEuler(data.rotation);
            entity.setScale(data.scale);
        } else {
            std::cerr << "Failed to apply transform to invalid entity: " << data.id << std::endl;
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

    void SceneLoader::processEntityChildren(Scene& scene, const EntityData& data, Entity parent, const std::string& prefixedId) {
        if (!data.childrenNode) return;

        for (pugi::xml_node childEntityNode : data.childrenNode.children("entity")) {
            EntityData childData = parseEntityData(childEntityNode);
            processEntity(scene, childData, parent, prefixedId);
        }
    }

    // ========== Template Handling ==========

    void SceneLoader::handleTemplateEntity(Scene& scene, const EntityData& data, Entity parent, const std::string& idPrefix) {
        std::string prefixedId = buildPrefixedId(data.id, idPrefix);

        auto templateAsset = mAssetManager.getTemplate(data.templateId);
        if (templateAsset && templateAsset->isReady()) {
            instantiateReadyTemplate(scene, data, prefixedId, parent);
        }
        else {
            queuePendingTemplate(data, prefixedId, idPrefix, parent, &scene);
        }
    }

    void SceneLoader::instantiateReadyTemplate(Scene& scene, const EntityData& data, const std::string& prefixedId, Entity parent) {
        Entity templateParent = createEntityFromData(scene, data, prefixedId);

        if (parent.isValid()) {
            templateParent.setParent(parent);
        }

        processEntityChildren(scene, data, templateParent, prefixedId);

        auto templateAsset = mAssetManager.getTemplate(data.templateId);
        templateAsset->instantiate(templateParent);

        logTemplateStatus(data.templateId, prefixedId, true);
    }

    void SceneLoader::queuePendingTemplate(const EntityData& data, const std::string& prefixedId,
        const std::string& idPrefix, Entity parent, Scene* scene) {
        std::lock_guard<std::mutex> lock(mPendingTemplatesMutex);
        mPendingTemplates.emplace_back(PendingTemplate{ data, prefixedId, idPrefix, parent, scene });
        logTemplateStatus(data.templateId, prefixedId, false);
    }

    void SceneLoader::processPendingTemplates() {
        std::lock_guard<std::mutex> lock(mPendingTemplatesMutex);

        auto it = mPendingTemplates.begin();
        while (it != mPendingTemplates.end()) {
            auto templateAsset = mAssetManager.getTemplate(it->entityData.templateId);
            if (templateAsset && templateAsset->isReady()) {
                instantiateReadyTemplate(*it->scene, it->entityData, it->prefixedId, it->parent);
                it = mPendingTemplates.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    // ========== Utility Functions ==========

    std::string SceneLoader::buildPrefixedId(const std::string& id, const std::string& idPrefix) {
        return idPrefix.empty() ? id : idPrefix + "." + id;
    }

    glm::vec3 SceneLoader::parseVec3(const std::string& str, const glm::vec3& defaultValue) {
        if (str.empty()) return defaultValue;

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

    void SceneLoader::logEntityCreation(const std::string& prefixedId, const std::string& name, bool isTemplate) {
        const char* entityType = isTemplate ? "template entity" : "entity";
        std::cout << "Created " << entityType << ": " << prefixedId << " (" << name << ")" << std::endl;
    }

    void SceneLoader::logTemplateStatus(const std::string& templateId, const std::string& prefixedId, bool instantiated) {
        if (instantiated) {
            std::cout << "Instantiated template: " << templateId << " as entity: " << prefixedId << std::endl;
        }
        else {
            std::cout << "Template " << templateId << " not ready, queued for instantiation as entity: " << prefixedId << std::endl;
        }
    }

}
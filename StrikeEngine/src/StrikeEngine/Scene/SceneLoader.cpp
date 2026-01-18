#include "strikepch.h"
#include "SceneLoader.h"
#include "Scene.h"
#include "Entity.h"
#include "ComponentRegistry.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "StrikeEngine/Asset/Types/Template.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Asset/Loaders/TemplateLoader.h"

namespace StrikeEngine {

    SceneLoader::SceneLoader() {
    }

    std::unique_ptr<Scene> SceneLoader::loadScene(const std::filesystem::path& path) {
        AssetManager::get().clear();
        return loadSceneInternal(path);
    }

    std::unique_ptr<Scene> SceneLoader::loadSceneInternal(const std::filesystem::path& path) {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(path.c_str());
        
        if (!result) {
            STRIKE_CORE_ERROR("Failed to load scene file '{}': {}", path.string(), result.description());
            return nullptr;
        }

        pugi::xml_node sceneNode = doc.child("scene");
        if (!sceneNode) {
            STRIKE_CORE_ERROR("Invalid scene format: no scene node found in file '{}'", path.string()); 
            return nullptr;
        }

        std::string sceneId = sceneNode.attribute("sceneId").as_string("DefaultScene");
        auto scene = std::make_unique<Scene>(sceneId, path);

        pugi::xml_node assetsNode = sceneNode.child("assets");
        if (assetsNode) {
            loadAssets(assetsNode, path.parent_path());
        }

        // Setup skybox and sun
        setupSkybox(*scene, sceneNode);
        setupSun(*scene, sceneNode);

        // Create entities (pass empty Entity() as root - entities without parent become roots)
        pugi::xml_node entitiesNode = sceneNode.child("entities");
        if (entitiesNode) {
            createEntities(*scene, entitiesNode, Entity());
        }

        return scene;
    }

    void SceneLoader::loadAssets(const pugi::xml_node& assetsNode, const std::filesystem::path& basePath) {
        auto& assetManager = AssetManager::get();
        for (pugi::xml_node assetNode : assetsNode.children()) {
            std::string assetId = assetNode.attribute("id").as_string();
            if (assetId.empty()) {
                continue;
            }
            assetManager.deserialize(assetNode, basePath, true);
        }

        while (assetManager.isLoading()) {  
            assetManager.update();     
        }
    }

    void SceneLoader::createEntities(Scene& scene, const pugi::xml_node& entitiesNode, Entity parentEntity) {
        for (pugi::xml_node entityNode : entitiesNode.children("entity")) {
            // Create entity - if parentEntity is invalid, it becomes a root entity
            Entity entity = parentEntity.isValid() ? scene.createEntity(parentEntity) : scene.createEntity();

            // Parse transform
            std::string posStr = entityNode.attribute("position").as_string("0,0,0");
            std::string rotStr = entityNode.attribute("rotation").as_string("0,0,0");
            std::string scaleStr = entityNode.attribute("scale").as_string("1,1,1");
            bool active = entityNode.attribute("active").as_bool(true);

            entity.setPosition(parseVector3(posStr));
            entity.setRotationEuler(parseVector3(rotStr));
            entity.setScale(parseVector3(scaleStr));
            entity.setActive(active);

            // Parse tag
            std::string tag = entityNode.attribute("tag").as_string("");
            if (!tag.empty()) {
                entity.setTag(tag);
            }

            // Check for template instantiation
            std::string templateId = entityNode.attribute("template").as_string("");
            if (!templateId.empty()) {
                auto templateAsset = AssetManager::get().getAsset<Template>(templateId);
                if (templateAsset && templateAsset->isReady()) {
                    templateAsset->instantiate(entity);
                }
            }

            // Parse components
            pugi::xml_node componentsNode = entityNode.child("components");
            if (componentsNode) {
                for (pugi::xml_node componentNode : componentsNode.children()) {
                    std::string componentType = componentNode.name();
                    
                    if (ComponentRegistry::hasComponentFactory(componentType)) {
                        ComponentRegistry::addComponentToEntity(entity, componentType, componentNode);
                    }
                }
            }

            // Recursively create child entities
            createEntities(scene, entityNode, entity);
        }
    }

    void SceneLoader::setupSkybox(Scene& scene, const pugi::xml_node& sceneNode) {
        pugi::xml_node skyboxNode = sceneNode.child("skybox");
        if (skyboxNode) {
            std::string cubeMapId = skyboxNode.attribute("cubeMapId").as_string();
            if (!cubeMapId.empty()) {
                scene.setSkybox(cubeMapId);
            }
        }
    }

    void SceneLoader::setupSun(Scene& scene, const pugi::xml_node& sceneNode) {
        pugi::xml_node sunNode = sceneNode.child("sun");
        if (sunNode) {
            Sun& sun = scene.getSun();

            std::string colorStr = sunNode.attribute("color").as_string("255,255,255");
            float intensity = sunNode.attribute("intensity").as_float(1.0f);
            std::string rotStr = sunNode.attribute("rotation").as_string("0,0,0");
            bool shadows = sunNode.attribute("shadows").as_bool(false);

            glm::vec3 color = parseVector3(colorStr);
            sun.setColor(color);
            sun.setIntensity(intensity);
            sun.setRotationEuler(parseVector3(rotStr));
            sun.setCastShadows(shadows);
        }
    }

    glm::vec3 SceneLoader::parseVector3(const std::string& str) {
        glm::vec3 result(0.0f);
        std::stringstream ss(str);
        char comma;
        ss >> result.x >> comma >> result.y >> comma >> result.z;
        return result;
    }

} // namespace StrikeEngine
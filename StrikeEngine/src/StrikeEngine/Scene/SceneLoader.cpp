#include "SceneLoader.h"
#include "Scene.h"
#include "Entity.h"
#include "ComponentRegistry.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "StrikeEngine/Asset/Types/Template.h"
#include "StrikeEngine/Asset/Types/Texture.h"
#include "StrikeEngine/Asset/Loaders/TemplateLoader.h"
#include "StrikeEngine/Scene/Components/TagComponent.h"
#include <stdexcept>
#include <sstream>

namespace StrikeEngine {

    SceneLoader::SceneLoader() : mIsLoading(false) {
    }

    std::future<std::unique_ptr<Scene>> SceneLoader::loadScene(const std::filesystem::path& path) {
        if (mIsLoading) {
            throw std::runtime_error("Scene is already loading");
        }

        mIsLoading = true;

        AssetManager::get().clear();
        
        std::promise<std::unique_ptr<Scene>> promise;
        promise.set_value(loadSceneInternal(path));

        mIsLoading = false;
        
        return promise.get_future();
    }

    std::future<std::unique_ptr<Scene>> SceneLoader::loadSceneAsync(const std::filesystem::path& path) {
        if (mIsLoading) {
            throw std::runtime_error("Scene is already loading");
        }

        mIsLoading = true;

        AssetManager::get().clear();
        
        
        return std::async(std::launch::async, [this, path]() {
            auto scene = loadSceneInternal(path, true);
            mIsLoading = false;
            return scene;
        });
    }

    bool SceneLoader::isLoading() const {
        return mIsLoading;
    }

    void SceneLoader::update() {

    }

    std::unique_ptr<Scene> SceneLoader::loadSceneInternal(const std::filesystem::path& path, bool async) {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(path.c_str());
        
        if (!result) {
            throw std::runtime_error("Failed to load scene XML: " + path.string());
        }

        pugi::xml_node sceneNode = doc.child("scene");
        if (!sceneNode) {
            throw std::runtime_error("Invalid scene format: no scene node found");
        }

        std::string sceneId = sceneNode.attribute("sceneId").as_string("DefaultScene");
        auto scene = std::make_unique<Scene>(sceneId, path);

        pugi::xml_node assetsNode = sceneNode.child("assets");
        if (assetsNode) {
            loadAssets(assetsNode, path.parent_path(), async);
        }

        // Step 3: Setup skybox and sun
        setupSkybox(*scene, sceneNode);
        setupSun(*scene, sceneNode);

        // Step 4: Create entities (pass empty Entity() as root - entities without parent become roots)
        pugi::xml_node entitiesNode = sceneNode.child("entities");
        if (entitiesNode) {
            createEntities(*scene, entitiesNode, Entity());
        }

        return scene;
    }

    void SceneLoader::loadAssets(const pugi::xml_node& assetsNode, const std::filesystem::path& basePath, bool async) {
        auto& assetManager = AssetManager::get();
        std::vector<std::string> loadingAssetIds;

        for (pugi::xml_node assetNode : assetsNode.children()) {
            std::string assetId = assetNode.attribute("id").as_string();
            
            if (assetId.empty()) {
                continue;
            }

            assetManager.deserialize(assetNode, basePath, true);
            loadingAssetIds.push_back(assetId);

        }


        // WAIT FOR ASYNC ASSETS TO BE READY
        while (!loadingAssetIds.empty()) {  
            if (!async) {
                assetManager.update(); // UPDATE SYNC METHODS REQUIRED TO INIT LIKE MESH OR MODEL TO BE FULLY READY
            }
            for (auto it = loadingAssetIds.begin(); it != loadingAssetIds.end(); ) {
                auto asset = assetManager.getAssetBase(*it);
                if (!asset || asset->isReady() || asset->hasFailed()) {
                    it = loadingAssetIds.erase(it);
                } else {
                    ++it;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));           
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

            entity.setPosition(parseVector3(posStr));
            entity.setRotationEuler(parseVector3(rotStr));
            entity.setScale(parseVector3(scaleStr));

            // Parse tag
            std::string tag = entityNode.attribute("tag").as_string("");
            if (!tag.empty()) {
                entity.addComponent<TagComponent>(tag);
            }

            // Check for template instantiation
            std::string templateId = entityNode.attribute("template").as_string("");
            if (!templateId.empty()) {
                auto templateAsset = AssetManager::get().getTemplate(templateId);
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
                scene.setSkyboxCubeMap(cubeMapId);
            }
        }
    }

    void SceneLoader::setupSun(Scene& scene, const pugi::xml_node& sceneNode) {
        pugi::xml_node sunNode = sceneNode.child("sun");
        if (sunNode) {
            auto sun = scene.getSun();
            if (sun) {
                std::string colorStr = sunNode.attribute("color").as_string("255,255,255");
                float intensity = sunNode.attribute("intensity").as_float(1.0f);
                std::string rotStr = sunNode.attribute("rotation").as_string("0,0,0");
                bool shadows = sunNode.attribute("shadows").as_bool(false);

                glm::vec3 color = parseVector3(colorStr);
                sun->setColor(color);
                sun->setIntensity(intensity);
                sun->setRotationEuler(parseVector3(rotStr));
                sun->setCastShadows(shadows);
            }
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
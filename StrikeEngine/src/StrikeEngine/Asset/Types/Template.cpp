#include "strikepch.h"
#include "Template.h"
#include "StrikeEngine/Scene/Scene.h"
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"


namespace StrikeEngine {

    Template::Template(const std::string& id, const std::filesystem::path& path)
        : Asset(id, path) {
    }

    void Template::instantiate(Entity parentEntity) {
        if (getLoadingState() != AssetLoadingState::Ready) {
            throw std::runtime_error("Template not loaded: " + getId());
        }

        pugi::xml_node templateNode = mDoc.child("template");
        if (!templateNode) {
            throw std::runtime_error("Invalid template format: " + getId());
        }

        Scene* scene = parentEntity.getScene();
        createEntities(*scene, templateNode.child("entities"), parentEntity);
    }

    bool Template::areAssetsReady() const {
        auto& assetManager = AssetManager::get();
        for (const auto& asset : mReferencedAssets) {
            if (assetManager.isAssetLoading(asset)) {
                return false;
            }
        }
        return true;
    }

    void Template::createEntities(Scene& scene, const pugi::xml_node& entitiesNode, Entity parentEntity) {
        for (pugi::xml_node entityNode = entitiesNode.child("entity"); entityNode; entityNode = entityNode.next_sibling("entity")) {
            std::string tag = entityNode.attribute("tag").as_string("Default");
            glm::vec3 localPos = parseVector3(entityNode.attribute("position").as_string("0.0,0.0,0.0"));
            glm::vec3 localRot = parseVector3(entityNode.attribute("rotation").as_string("0.0,0.0,0.0"));
            glm::vec3 localScale = parseVector3(entityNode.attribute("scale").as_string("1.0,1.0,1.0"));

            Entity entity = scene.createEntity();
            entity.setTag(tag);
            entity.setPosition(localPos);
            entity.setRotationEuler(localRot);
            entity.setScale(localScale);

            if (parentEntity.isValid()) {
                entity.setParent(parentEntity);
            }

            pugi::xml_node componentsNode = entityNode.child("components");
            if (componentsNode) {
                for (pugi::xml_node componentNode : componentsNode.children()) {
                    std::string componentType = componentNode.name();
                    if (ComponentRegistry::hasComponentFactory(componentType)) {
                        ComponentRegistry::addComponentToEntity(entity, componentType, componentNode);
                        std::cout << "Added component: " << componentType 
                                << " to entity with tag: " << entity.getTag() 
                                << " in template: " << getId() << std::endl;
                    }
                    else {
                        std::cerr << "Unknown component type: " << componentType 
                                << " in entity with tag: " << entity.getTag() << std::endl;
                    }
                }
            }

            createEntities(scene, entityNode, entity);

            std::cout << "Created entity with tag: " << tag 
                    << " in template: " << getId() << std::endl;
        }
    }

    glm::vec3 Template::parseVector3(const std::string& str) {
        glm::vec3 vec(0.0f);
        if (str.empty()) {
            return vec;
        }
        std::sscanf(str.c_str(), "%f,%f,%f", &vec.x, &vec.y, &vec.z);
        return vec;
    }

    void Template::setTemplateDoc(const pugi::xml_document& doc) {
        mDoc.reset(doc);
    }


}
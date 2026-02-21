#include "strikepch.h"
#include "Template.h"
#include "StrikeEngine/Scene/Scene.h"
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"


namespace Strike {

    Template::Template(const std::string& id, const std::filesystem::path& path)
        : Asset(id, path) {
    }

    bool Template::instantiate(Entity parentEntity) {
        if (!isReady()) {
            STRIKE_CORE_WARN("Template '{}' is not ready for instantiation", getId());
            return false;
        }

        pugi::xml_node templateNode = mDoc.child("template");
        if (!templateNode) {
            STRIKE_CORE_ERROR("Invalid template format in '{}': no template node found", getId());
            mLoadingState = AssetState::Failed;
            return false;
        }

        Scene* scene = parentEntity.getScene();
        createEntities(*scene, templateNode.child("entities"), parentEntity);
        return true;
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
            Entity entity = scene.createEntity();
            std::string posStr = entityNode.attribute("position").as_string("0,0,0");
            std::string rotStr = entityNode.attribute("rotation").as_string("0,0,0");
            std::string scaleStr = entityNode.attribute("scale").as_string("1,1,1");
            bool active = entityNode.attribute("active").as_bool(true);

            entity.setPosition(parseVector3(posStr));
            entity.setEulerAngles(parseVector3(rotStr));
            entity.setScale(parseVector3(scaleStr));
            entity.setActive(active);
            
            // Warn on template nesting
            if (entityNode.attribute("template")) {
                STRIKE_CORE_WARN("Template '{}': template nesting is not supported — 'template' attribute on entity will be ignored", getId());
            }

            // Parse tag
            std::string tag = entityNode.attribute("tag").as_string("");
            if (!tag.empty()) {
                entity.setTag(tag);
            }

            if (parentEntity.isValid()) {
                entity.setParent(parentEntity);
            }

            pugi::xml_node componentsNode = entityNode.child("components");
            if (componentsNode) {
                for (pugi::xml_node componentNode : componentsNode.children()) {
                    std::string componentType = componentNode.name();
                    if (ComponentRegistry::hasComponentFactory(componentType)) {
                        ComponentRegistry::addComponentToEntity(entity, componentType, componentNode);
                    }
                    else {
                        STRIKE_CORE_ERROR("Unknown component type '{}' in template '{}'", 
                                componentType, getId());
                    }
                }
            }

            createEntities(scene, entityNode, entity);
        }
    }

    glm::vec3 Template::parseVector3(const std::string& str) {
        glm::vec3 result(0.0f);
        std::stringstream ss(str);
        char comma;
        ss >> result.x >> comma >> result.y >> comma >> result.z;
        return result;
    }

    void Template::setTemplateDoc(const pugi::xml_document& doc) {
        mDoc.reset(doc);
    }


}
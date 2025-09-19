#include "Template.h"
#include <stdexcept>
#include "StrikeEngine/Asset/AssetManager.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

namespace StrikeEngine {

    Template::Template(const std::string& id, const std::filesystem::path& path, const std::string& name)
        : Asset(id, path, name) {
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
        std::string idPrefix = parentEntity.getId();

        createEntities(*scene, templateNode.child("entities"), parentEntity, idPrefix);
    }

    void Template::createEntities(Scene& scene, const pugi::xml_node& entitiesNode, Entity parentEntity, const std::string& idPrefix) {
        ComponentRegistry& componentRegistry = ComponentRegistry::get();

        for (pugi::xml_node entityNode = entitiesNode.child("entity"); entityNode; entityNode = entityNode.next_sibling("entity")) {
            std::string id = entityNode.attribute("id").as_string();
            std::string name = entityNode.attribute("name").as_string();

            if (id.empty()) {
                throw std::runtime_error("Entity missing id attribute in template: " + getId());
            }

            std::string prefixedId = idPrefix.empty() ? id : idPrefix + "." + id;
            if (name.empty()) {
                name = id; 
            }

            glm::vec3 localPos = parseVector3(entityNode.attribute("position").as_string("0.0,0.0,0.0"));
            glm::vec3 localRot = parseVector3(entityNode.attribute("rotation").as_string("0.0,0.0,0.0"));
            glm::vec3 localScale = parseVector3(entityNode.attribute("scale").as_string("1.0,1.0,1.0"));

            // Apply parent transform
            /*
            glm::quat localQuat = glm::quat(glm::radians(localRot));
            localPos = parentEntity.getPosition() + parentEntity.getRotation() * (parentEntity.getScale() * localPos);
            localQuat = parentEntity.getRotation() * localQuat;
            localScale *= parentEntity.getScale();
            */

            Entity entity = Entity::create(&scene, prefixedId);
            entity.setName(name);
            entity.setPosition(localPos);
            entity.setEulerRotation(localRot);
            entity.setScale(localScale);

            if (parentEntity.isValid()) {
                entity.setParent(parentEntity);
            }

            pugi::xml_node componentsNode = entityNode.child("components");
            if (componentsNode) {
                for (pugi::xml_node componentNode : componentsNode.children()) {
                    std::string componentType = componentNode.name();

                    if (componentRegistry.isRegistered(componentType)) {
                        componentRegistry.addComponentToEntity(entity, componentType, componentNode);
                        std::cout << "Added component: " << componentType << " to entity: " << entity.getId() << " in template: " << getId() << std::endl;
                    }
                    else {
                        std::cerr << "Unknown component type: " << componentType << " in entity: " << entity.getId() << std::endl;
                    }
                }
            }

            pugi::xml_node childrenNode = entityNode.child("children");
            if (childrenNode) {
                createEntities(scene, childrenNode, entity, idPrefix);
            }

            std::cout << "Created entity: " << prefixedId << " (" << name << ") in template: " << getId() << std::endl;
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
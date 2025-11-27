#include "HierarchyComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"
#include "StrikeEngine/Scene/Entity.h"
#include "StrikeEngine/Scene/Scene.h"

#include <algorithm>

namespace StrikeEngine {

    HierarchyComponent::HierarchyComponent(entt::entity parent)
        : mParent(parent) {}

    const std::string& HierarchyComponent::getStaticTypeName() {
        static const std::string name = "HierarchyComponent";
        return name;
    }

    const std::string& HierarchyComponent::getTypeName() const {
        return getStaticTypeName();
    }

    Entity HierarchyComponent::getParent() const {
        return Entity(mParent, mScene);
    }


    std::vector<Entity> HierarchyComponent::getChildren() const {
        std::vector<Entity> result;
        for (entt::entity child : mChildren) {
            result.emplace_back(child, mScene);
        }
        return result;
    }

    size_t HierarchyComponent::getChildCount() const {
       return mChildren.size();
    }

    bool HierarchyComponent::hasChildren() const {
       return mChildren.empty();
    }

    void HierarchyComponent::setParent(Entity parent) {
        Entity entObj(mEntity, mScene);
        mScene->setParent(entObj, parent);
    }

    void HierarchyComponent::addChild(Entity child) {
        Entity entObj(mEntity, mScene);
        mScene->addChild(entObj, child);
        
    }

    void HierarchyComponent::deserialize(const pugi::xml_node& node) {
        // Implement deserialization if needed
    }

    void HierarchyComponent::serialize(pugi::xml_node& node) const {
        // Implement serialization if needed
    }

    bool HierarchyComponent::isRoot() const{
        return mScene->getRoot().getHandle() == mEntity;
    }

}
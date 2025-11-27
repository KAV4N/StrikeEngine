#include "Entity.h"

#include "Components/TransformComponent.h"
#include "Components/HierarchyComponent.h"
#include "Components/TagComponent.h"

#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <stdexcept>

namespace StrikeEngine {

    Entity::Entity(entt::entity handle, Scene* scene)
        : mHandle(handle), mScene(scene) {}

    TransformComponent& Entity::getTransform() { return getComponent<TransformComponent>(); }
    const TransformComponent& Entity::getTransform() const { return getComponent<TransformComponent>(); }

    // Essential component accessors
    HierarchyComponent& Entity::getHierarchy() { return getComponent<HierarchyComponent>(); }
    const HierarchyComponent& Entity::getHierarchy() const { return getComponent<HierarchyComponent>(); }
    
    void Entity::setTag(const std::string& tag) { getOrAddComponent<TagComponent>().setTag(tag); }
    const std::string& Entity::getTag() const { return getComponent<TagComponent>().getTag(); }
    
    bool Entity::isValid() const {
        return mScene && mScene->getRegistry().valid(mHandle);
    }

    void Entity::setParent(Entity parent) {
        if (!isValid() || !parent.isValid()) {
            return;
        }
        getHierarchy().setParent(parent);
    }

    void Entity::addChild(Entity child){
        if (!isValid() || !child.isValid()) {
            return;
        }
        getHierarchy().addChild(child);
    }

    Entity Entity::getParent() const {
        if (!isValid()) {
            return Entity();
        }
        return getHierarchy().getParent();
    }

    std::vector<Entity> Entity::getChildren() const {
        if (!isValid()) {
            return std::vector<Entity>();
        }
        return getHierarchy().getChildren();
    }


    bool Entity::isRoot() const {
        if (!isValid()) {
            return false;
        }
        return mScene->getRoot().mHandle == mHandle;
    }

} // namespace StrikeEngine
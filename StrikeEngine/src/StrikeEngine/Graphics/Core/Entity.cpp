#include "Entity.h"
#include "Scene.h"

namespace StrikeEngine {
    Entity::Entity(Scene* scene, entt::registry& registry, const std::string& id, const std::string& name)
        : GraphNode(scene, registry.create(), id, name), m_registry(registry), m_entityHandle(GraphNode::GetEntityHandle()) {
    }

    Entity::~Entity() {
        if (m_registry.valid(m_entityHandle)) {
            m_registry.destroy(m_entityHandle);
        }
    }

    void Entity::AddChildEntity(entt::entity childId) {
        if (childId == entt::null) return;
        AddChild(childId);
    }

    void Entity::RemoveChildEntity(entt::entity childId) {
        if (childId == entt::null) return;
        RemoveChild(childId);
    }

    std::shared_ptr<Entity> Entity::GetParentEntity() const {
        if (m_parent == entt::null) return nullptr;
        return m_scene->GetEntityByHandle(m_parent);
    }

    std::vector<std::shared_ptr<Entity>> Entity::GetChildEntities() const {
        std::vector<std::shared_ptr<Entity>> entityChildren;
        for (const auto& childId : m_children) {
            auto childEntity = m_scene->GetEntityByHandle(childId);
            if (childEntity) {
                entityChildren.push_back(childEntity);
            }
        }
        return entityChildren;
    }
}
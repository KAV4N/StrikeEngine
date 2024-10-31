#include "strikepch.h"
#include "Entity.h"
#include "Scene.h"
#include "SceneGraph.h"

namespace StrikeEngine {

    Entity::Entity(entt::entity handle, Scene* scene)
        : m_EntityHandle(handle), m_Scene(scene)
    {
    }

    bool Entity::SetParent(Entity parent)
    {
        if (!m_Scene)
            return false;

        return m_Scene->GetSceneGraph().SetParent(*this, parent);
    }

    Entity Entity::GetParent() const
    {
        if (!m_Scene)
            return Entity();

        SceneNode* node = m_Scene->GetSceneGraph().GetNode(*this);
        return node && node->parent ? node->parent->entity : Entity();
    }

    bool Entity::AddChild(Entity child)
    {
        if (!m_Scene)
            return false;

        return m_Scene->GetSceneGraph().AddChild(*this, child);
    }

    bool Entity::RemoveChild(Entity child)
    {
        if (!m_Scene)
            return false;

        return m_Scene->GetSceneGraph().RemoveChild(*this, child);
    }

    std::vector<Entity> Entity::GetChildren() const
    {
        if (!m_Scene)
            return {};

        return m_Scene->GetSceneGraph().GetChildren(*this);
    }

    std::vector<Entity> Entity::GetAllDescendants() const
    {
        if (!m_Scene)
            return {};

        return m_Scene->GetSceneGraph().GetAllDescendants(*this);
    }

    bool Entity::Destroy()
    {
        if (!m_Scene)
            return false;

        return m_Scene->GetSceneGraph().RemoveNode(*this);
    }
    bool Entity::operator==(const Entity& other) const
    {
        return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
    }

    bool Entity::operator!=(const Entity& other) const
    {
        return !(*this == other);
    }

    Entity::operator bool() const
    {
        return m_EntityHandle != entt::null && m_Scene;
    }

}

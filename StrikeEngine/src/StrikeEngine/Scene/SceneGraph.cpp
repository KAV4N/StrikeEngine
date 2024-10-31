#include "strikepch.h"
#include "SceneGraph.h"
#include "Scene.h"
#include <queue>

namespace StrikeEngine {

    SceneGraph::SceneGraph(Scene* scene, Entity rootEntity)
        : m_Scene(scene)
    {

        m_Root = new SceneNode{ rootEntity };
        m_NodeMap[rootEntity.GetHandle()] = m_Root;
        
    }

    SceneGraph::~SceneGraph()
    {
        DeleteNode(m_Root);
    }

    SceneNode* SceneGraph::GetNode(Entity entity) const
    {
        auto it = m_NodeMap.find(entity.GetHandle());
        return it != m_NodeMap.end() ? it->second : nullptr;
    }

    bool SceneGraph::SetParent(Entity child, Entity parent)
    {
        SceneNode* childNode = GetNode(child);
        SceneNode* parentNode = GetNode(parent);

        if (!childNode || !parentNode || childNode == parentNode)
            return false;

        // Check for circular reference
        SceneNode* node = parentNode;
        while (node) {
            if (node == childNode)
                return false;
            node = node->parent;
        }

        // Remove from current parent
        if (childNode->parent) {
            auto& siblings = childNode->parent->children;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), childNode), siblings.end());
        }

        // Set new parent
        childNode->parent = parentNode;
        parentNode->children.push_back(childNode);

        return true;
    }

    bool SceneGraph::AddChild(Entity parent, Entity child)
    {
        return SetParent(child, parent);
    }

    bool SceneGraph::RemoveChild(Entity parent, Entity child)
    {
        SceneNode* parentNode = GetNode(parent);
        SceneNode* childNode = GetNode(child);

        if (!parentNode || !childNode || childNode->parent != parentNode)
            return false;

        auto& children = parentNode->children;
        auto it = std::find(children.begin(), children.end(), childNode);
        if (it != children.end()) {
            children.erase(it);
            childNode->parent = nullptr;
            return true;
        }

        return false;
    }

    std::vector<Entity> SceneGraph::GetChildren(Entity entity) const
    {
        std::vector<Entity> children;
        SceneNode* node = GetNode(entity);
        if (node) {
            for (SceneNode* child : node->children) {
                children.push_back(child->entity);
            }
        }
        return children;
    }

    std::vector<Entity> SceneGraph::GetAllDescendants(Entity entity) const
    {
        std::vector<Entity> descendants;
        SceneNode* node = GetNode(entity);
        if (node) {
            GetAllDescendantsRecursive(node, descendants);
        }
        return descendants;
    }

    void SceneGraph::GetAllDescendantsRecursive(SceneNode* node, std::vector<Entity>& descendants) const
    {
        for (SceneNode* child : node->children) {
            descendants.push_back(child->entity);
            GetAllDescendantsRecursive(child, descendants);
        }
    }

    SceneNode* SceneGraph::CreateNode(Entity entity, Entity parent)
    {
        SceneNode* node = new SceneNode{ entity };
        m_NodeMap[entity.GetHandle()] = node;

        if (!parent) {
            parent = m_Root->entity;
        }
        AddChild(parent, entity);

        return node;
    }

    bool SceneGraph::RemoveNode(Entity entity)
    {
        SceneNode* node = GetNode(entity);
        if (!node || node == m_Root) {
            return false;
        }

        DeleteNode(node);
        return true;
    }

    void SceneGraph::DeleteNode(SceneNode* startNode) {
        if (!startNode) return;

        for (SceneNode* child : startNode->children) {
            if (child) {
                DeleteNode(child);
            }
        }

        if (startNode->parent) {
            SceneNode* parent = startNode->parent;
            for (size_t i = 0; i < parent->children.size(); ++i) {
                if (parent->children[i] == startNode) {
                    parent->children[i] = parent->children.back();
                    parent->children.pop_back();
                    break;
                }
            }
        }

        if (m_Scene) {
            m_Scene->m_Registry.destroy(startNode->entity.GetHandle());
        }
        m_NodeMap.erase(startNode->entity.GetHandle());

        delete startNode;
    }

}

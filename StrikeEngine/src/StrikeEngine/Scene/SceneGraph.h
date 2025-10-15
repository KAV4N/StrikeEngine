#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include "Entity.h"

namespace StrikeEngine {

    class Scene;

    struct SceneNode {
        Entity entity;
        SceneNode* parent = nullptr;
        std::vector<SceneNode*> children;
    };

    class SceneGraph {
    public:
        SceneGraph(Scene* scene, Entity rootEntity);
        ~SceneGraph();

        SceneNode* GetRoot() const { return m_Root; }
        SceneNode* GetNode(Entity entity) const;
        bool SetParent(Entity child, Entity parent);
        bool AddChild(Entity parent, Entity child);
        bool RemoveChild(Entity parent, Entity child);
        std::vector<Entity> GetChildren(Entity entity) const;
        std::vector<Entity> GetAllDescendants(Entity entity) const;
        bool RemoveNode(Entity entity);
        SceneNode* GetRootNode() {
            return m_Root;
        }
    private:
        friend class Scene;
        friend class Entity;

        SceneNode* CreateNode(Entity entity, Entity parent=Entity());
        void DeleteNode(SceneNode* node);
        void GetAllDescendantsRecursive(SceneNode* node, std::vector<Entity>& descendants) const;

        Scene* m_Scene;
        SceneNode* m_Root;
        std::unordered_map<entt::entity, SceneNode*> m_NodeMap;
    };

}

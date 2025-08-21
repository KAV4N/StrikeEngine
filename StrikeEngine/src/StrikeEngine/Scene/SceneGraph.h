#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "Entity.h"

namespace StrikeEngine {

    class Entity;

    class GraphNode : public std::enable_shared_from_this<GraphNode> {
    public:
        GraphNode(Entity entity, const std::string& id, const std::string& name, bool isRoot = false);

        // Accessors
        Entity getEntity() const { return mEntity; }
        const std::string& getId() const { return mId; }
        const std::string& getName() const { return mName; }
        const glm::vec3& getPosition() const { return mPosition; }
        const glm::quat& getRotation() const { return mRotation; }
        const glm::vec3& getScale() const { return mScale; }
        std::shared_ptr<GraphNode> getParent() const { return mParent.lock(); }
        const std::vector<std::shared_ptr<GraphNode>>& getChildren() const { return mChildren; }

        bool isDirty() const { return mIsDirty; }
        bool isRoot() const { return mIsRootNode; }
        bool isActive() const { return mIsActive; }

        // Mutators
        void setPosition(const glm::vec3& pos) { mPosition = pos; markDirty(); }
        void setRotation(const glm::quat& rot) { mRotation = rot; markDirty(); }
        void setScale(const glm::vec3& scl) { mScale = scl; markDirty(); }
        void setParent(const std::shared_ptr<GraphNode>& newParent) {
            if (!mIsRootNode) {
                mParent = newParent;
                markDirty();
            }
        }

        void addChild(const std::shared_ptr<GraphNode>& child) {
            mChildren.push_back(child);
            child->setParent(shared_from_this());
        }

        void removeChild(const std::shared_ptr<GraphNode>& child) {
            mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), child), mChildren.end());
        }

        void markDirty() { mIsDirty = true; }
        void clearDirty() { mIsDirty = false; }
        void setActive(bool active) { mIsActive = active; }

    private:
        Entity mEntity;
        std::string mId;
        std::string mName;
        glm::vec3 mPosition;
        glm::quat mRotation;
        glm::vec3 mScale;

        std::weak_ptr<GraphNode> mParent;
        std::vector<std::shared_ptr<GraphNode>> mChildren;

        bool mIsActive = true;
        bool mIsDirty = true;
        bool mIsRootNode = false;
    };

    class SceneGraph {
    public:
        SceneGraph();
        ~SceneGraph();

        // Node management
        void createRootNode(Entity entity, const std::string& id, const std::string& name);
        void createNode(Entity entity, const std::string& id, const std::string& name);
        void removeNode(Entity entity);
        bool hasNode(Entity entity) const;
        bool isRootNode(Entity entity) const;
        bool isActive(Entity entity) const;

        // Hierarchy operations
        void setParent(Entity child, Entity parent);
        void removeParent(Entity child);
        void addChild(Entity parent, Entity child);
        void removeChild(Entity parent, Entity child);

        // Transform operations
        void setPosition(Entity entity, const glm::vec3& position);
        void setRotation(Entity entity, const glm::quat& rotation);
        void setScale(Entity entity, const glm::vec3& scale);

        glm::vec3 getPosition(Entity entity) const;
        glm::quat getRotation(Entity entity) const;
        glm::vec3 getScale(Entity entity) const;

        // World transform calculations
        glm::vec3 getWorldPosition(Entity entity) const;
        glm::quat getWorldRotation(Entity entity) const;
        glm::vec3 getWorldScale(Entity entity) const;
        glm::mat4 getWorldMatrix(Entity entity) const;

        // Query operations
        Entity getEntityByName(const std::string& name) const;
        Entity getEntityById(const std::string& id) const;
        std::string getEntityName(Entity entity) const;
        std::string getEntityId(Entity entity) const;
        std::vector<Entity> getChildren(Entity entity) const;
        Entity getParent(Entity entity) const;

        // Hierarchy queries
        bool isAncestor(Entity ancestor, Entity descendant) const;
        bool isDescendant(Entity descendant, Entity ancestor) const;
        std::vector<Entity> getRootEntities() const;
        Entity getRootEntity() const;

        // Update operations
        void markDirty(Entity entity);
        void clearDirty(Entity entity);
        bool isDirty(Entity entity) const;
        void updateTransforms();

        // Management
        void clear();

    private:
        std::unordered_map<Entity, std::shared_ptr<GraphNode>> nodes;
        std::shared_ptr<GraphNode> rootNode;

        std::shared_ptr<GraphNode> getNode(Entity entity) const;
    };
}
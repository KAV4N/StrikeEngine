#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "Entity.h"

namespace StrikeEngine {

    class Scene;
    class Entity;

    class GraphNode : public std::enable_shared_from_this<GraphNode> {
    public:
        GraphNode(Entity entity, bool isRoot = false);

        // Accessors
        Entity getEntity() const { return mEntity; }

        const glm::vec3& getPosition() const { return mPosition; }
        float getPositionX() const { return mPosition.x; }
        float getPositionY() const { return mPosition.y; }
        float getPositionZ() const { return mPosition.z; }

        const glm::quat& getRotation() const { return mRotation; }
        glm::vec3 getEulerRotation();
        float getRotationX() { return getEulerRotation().x; }
        float getRotationY() { return getEulerRotation().y; }
        float getRotationZ() { return getEulerRotation().z; }

        const glm::vec3& getScale() const { return mScale; }
        float getScaleX() const { return mScale.x; }
        float getScaleY() const { return mScale.y; }
        float getScaleZ() const { return mScale.z; }

        const glm::mat4& getLocalMatrix() const { return mLocalMatrix; }
        const glm::mat4& getWorldMatrix() const { return mWorldMatrix; }

        std::shared_ptr<GraphNode> getParent() const { return mParent.lock(); }
        const std::vector<std::shared_ptr<GraphNode>>& getChildren() const { return mChildren; }

        bool isRoot() const { return mIsRootNode; }
        bool isActive() const { return mIsActive; }

        // Mutators
        void setPosition(const glm::vec3& pos) { mPosition = pos; mIsDirty = true; }
        void setPositionX(float x) { mPosition.x = x; mIsDirty = true; }
        void setPositionY(float y) { mPosition.y = y; mIsDirty = true; }
        void setPositionZ(float z) { mPosition.z = z; mIsDirty = true; }
        void move(const glm::vec3& offset) { mPosition += offset; mIsDirty = true; }
        void moveX(float x) { mPosition.x += x; mIsDirty = true; }
        void moveY(float y) { mPosition.y += y; mIsDirty = true; }
        void moveZ(float z) { mPosition.z += z; mIsDirty = true; }

        void setRotation(const glm::quat& rot) { mRotation = rot; mIsDirty = true; }
        void setEulerRotation(const glm::vec3& eulerAngles);
        void setRotationX(float angleDegrees);
        void setRotationY(float angleDegrees);
        void setRotationZ(float angleDegrees);
        void rotate(const glm::quat& rotation) { mRotation = glm::normalize(rotation * mRotation); mIsDirty = true; }
        void rotateEuler(const glm::vec3& anglesDegrees);
        void rotateX(float angleDegrees);
        void rotateY(float angleDegrees);
        void rotateZ(float angleDegrees);

        void setScale(const glm::vec3& scl) { mScale = scl; mIsDirty = true; }
        void setScaleX(float x) { mScale.x = x; mIsDirty = true; }
        void setScaleY(float y) { mScale.y = y; mIsDirty = true; }
        void setScaleZ(float z) { mScale.z = z; mIsDirty = true; }
        void scaleBy(const glm::vec3& factor) { mScale *= factor; mIsDirty = true; }
        void scaleByX(float x) { mScale.x *= x; mIsDirty = true; }
        void scaleByY(float y) { mScale.y *= y; mIsDirty = true; }
        void scaleByZ(float z) { mScale.z *= z; mIsDirty = true; }

        void setActive(bool active) { mIsActive = active; }
        const std::string& getEntityId() const { return mEntity.getId(); }

    private:
        friend class SceneGraph;
        void setParent(const std::shared_ptr<GraphNode>& newParent);
        void addChild(const std::shared_ptr<GraphNode>& child);
        void removeChild(const std::shared_ptr<GraphNode>& child);

        float normalizeAngle(float angle);
        glm::vec3 normalizeEulerAngles(const glm::vec3& angles);

        void updateLocalMatrix();
        void updateWorldMatrix(const glm::mat4& parentWorldMatrix = glm::mat4(1.0f));

    private:
        Entity mEntity;

        glm::vec3 mPosition;
        glm::quat mRotation;
        glm::vec3 mScale;

        glm::mat4 mLocalMatrix;
        glm::mat4 mWorldMatrix;
        bool mIsDirty = true;

        std::weak_ptr<GraphNode> mParent;
        std::vector<std::shared_ptr<GraphNode>> mChildren;

        bool mIsActive = true;
        bool mIsRootNode = false;
    };

    class SceneGraph {
    public:
        SceneGraph(Scene* scene, const std::string& rootId, const std::string& rootName = "");
        ~SceneGraph() = default;

        // Entity creation and retrieval
        
        Entity createEntity(const std::string& id, const std::string& parentId = "");
        Entity getEntity(const std::string& id);
        std::vector<Entity> getEntitiesByName(const std::string& name) const;

        // Retrieve all entities with a specific component
        template<typename Component>
        std::vector<Entity> getEntitiesWithComponent() const {
            std::vector<Entity> result;
            for (const auto& pair : mNodes) {
                const auto& node = pair.second;
                if (node && node->getEntity().isValid()) {
                    if (mRegistry.all_of<Component>(node->getEntity().mHandle)) {
                        result.push_back(node->getEntity());
                    }
                }
            }
            return result;
        }

        // Entity destruction
        bool destroyEntity(const std::string& id);

        // Root entity operations
        Entity getRootEntity() const;
        bool isRoot(const std::string& id) const;
        bool isActive(const std::string& id) const;

        // Entity existence check
        bool entityExists(const std::string& id);

        // Hierarchy operations
        void setParent(const std::string& childId, const std::string& parentId);
        void addChild(const std::string& parentId, const std::string& childId);
        void removeChild(const std::string& parentId, const std::string& childId);

        // Query operations
        std::vector<Entity> getChildren(const std::string& id) const;
        Entity getParent(const std::string& id) const;

        // Hierarchy queries
        bool isAncestor(const std::string& ancestorId, const std::string& descendantId) const;
        bool isDescendant(const std::string& descendantId, const std::string& ancestorId) const;

        // Update operations
        // Update operations
        void updateTransforms();

        // Management
        void clear();

    private:
        friend class Scene;
        friend class Entity;

        // Node access (internal)
        std::shared_ptr<GraphNode> getNode(const std::string& id) const;
        void updateNodeTransforms(std::shared_ptr<GraphNode> node);
        Entity createEntityInternal(const std::string& id);
        void createRootEntity(const std::string& rootId, const std::string& rootName = "");

    private:
        entt::registry mRegistry;
        Scene* mScene;
        std::unordered_map<std::string, std::shared_ptr<GraphNode>> mNodes;
        std::pair<std::string, std::shared_ptr<GraphNode>> mRootNode;
    };
}

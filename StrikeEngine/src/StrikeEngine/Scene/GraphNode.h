#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <entt/entt.hpp>
#include <memory>
#include <vector>
#include <string>

namespace StrikeEngine {

    // Internal class - not exposed to users
    class GraphNode : public std::enable_shared_from_this<GraphNode> {
    public:
        GraphNode(entt::entity entityId);
        ~GraphNode() = default;

        GraphNode(const GraphNode&) = delete;
        GraphNode& operator=(const GraphNode&) = delete;
        GraphNode(GraphNode&&) = delete;
        GraphNode& operator=(GraphNode&&) = delete;

        // Entity association
        entt::entity getEntityId() const { return mEntityId; }

        // Transform operations
        const glm::vec3& getLocalPosition() const { return mLocalPosition; }
        void setLocalPosition(const glm::vec3& position);
        void move(const glm::vec3& offset);

        const glm::quat& getLocalRotation() const { return mLocalRotation; }
        void setLocalRotation(const glm::quat& rotation);
        void setLocalRotationEuler(const glm::vec3& eulerAngles);
        void rotateEuler(const glm::vec3& anglesDegrees);
        void rotateQuaternion(float angleDegrees, const glm::vec3& axis);
        glm::vec3 getLocalRotationEuler() const;

        const glm::vec3& getLocalScale() const { return mLocalScale; }
        void setLocalScale(const glm::vec3& scale);
        void scale(const glm::vec3& factor);

        // Matrix operations
        const glm::mat4& getLocalMatrix() const;
        const glm::mat4& getWorldMatrix() const { return mWorldMatrix; }

        // Hierarchy operations
        GraphNode* getParent() const { 
            auto parent = mParent.lock();
            return parent ? parent.get() : nullptr;
        }
        void setParent(std::shared_ptr<GraphNode> parent);
        void removeParent();

        void addChild(std::shared_ptr<GraphNode> child);
        void removeChild(GraphNode* child);
        const std::vector<std::shared_ptr<GraphNode>>& getChildren() const { return mChildren; }
        size_t getChildCount() const { return mChildren.size(); }
        bool hasChildren() const { return !mChildren.empty(); }
        bool isRoot() const { return mParent.expired(); }

        // Dirty flag
        bool isDirty() const { return mIsDirty; }
        void markDirty();
        void clearDirty() { mIsDirty = false; }

        // Tag operations (now stored directly in GraphNode)
        const std::string& getTag() const { return mTag; }
        void setTag(const std::string& tag) { mTag = tag; }

    private:
        friend class Scene;
        void updateLocalMatrix() const;

    private:
        entt::entity mEntityId;

        std::string mTag;  // Tag is now part of the graph node itself

        // Transform data
        glm::vec3 mLocalPosition{0.0f};
        glm::quat mLocalRotation{1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 mLocalScale{1.0f};

        mutable bool mIsDirty = true;
        mutable glm::mat4 mLocalMatrix{1.0f};
        mutable glm::mat4 mWorldMatrix{1.0f};

        // Hierarchy data
        std::weak_ptr<GraphNode> mParent;
        std::vector<std::shared_ptr<GraphNode>> mChildren;
    };

} // namespace StrikeEngine
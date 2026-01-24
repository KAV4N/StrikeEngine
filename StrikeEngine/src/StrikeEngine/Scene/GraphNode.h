#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <entt/entt.hpp>

#include <memory>
#include <vector>
#include <string>

namespace StrikeEngine {

    class GraphNode : public std::enable_shared_from_this<GraphNode> {
    public:
        explicit GraphNode(entt::entity entityId);
        ~GraphNode() = default;

        GraphNode(const GraphNode&) = delete;
        GraphNode& operator=(const GraphNode&) = delete;

        entt::entity getEntityId() const { return mEntityId; }

        // ===============================
        // Local Transform 
        // ===============================
        const glm::vec3& getPosition() const { return mPosition; }
        void setPosition(const glm::vec3& position);
        void move(const glm::vec3& delta);

        const glm::quat& getRotation() const { return mRotation; }
        void setRotation(const glm::quat& rotation);
        void rotate(const glm::vec3& angles);
        void rotateQuaternion(float angleDegrees, const glm::vec3& axis);

        glm::vec3 getEulerAngles() const;
        void setEulerAngles(const glm::vec3& angles);

        const glm::vec3& getScale() const { return mScale; }
        void setScale(const glm::vec3& scale);

        // ===============================
        // World Transform
        // ===============================
        glm::vec3 getWorldPosition() const;
        void setWorldPosition(const glm::vec3& position);

        glm::quat getWorldRotation() const;
        void setWorldRotation(const glm::quat& rotation);

        glm::vec3 getWorldEulerAngles() const;
        void setWorldEulerAngles(const glm::vec3& angles);

        glm::vec3 getWorldScale() const;

        // ===============================
        // Direction vectors (world space)
        // ===============================
        glm::vec3 getForward() const;
        glm::vec3 getRight() const;
        glm::vec3 getUp() const;

        // ===============================
        // Matrices
        // ===============================
        const glm::mat4& getLocalMatrix() const;
        const glm::mat4& getWorldMatrix() const;

        // ===============================
        // Hierarchy
        // ===============================
        GraphNode* getParent() const;
        void setParent(std::shared_ptr<GraphNode> parent);
        void removeFromParent();

        void addChild(std::shared_ptr<GraphNode> child);
        const std::vector<std::shared_ptr<GraphNode>>& getChildren() const { return mChildren; }

        // ===============================
        // Utility
        // ===============================
        const std::string& getTag() const { return mTag; }
        void setTag(const std::string& tag) { mTag = tag; }

        bool isActive() const { return mIsActive; }
        void setActive(bool active) { mIsActive = active; }

        void updateLocalMatrix() const;
        void updateWorldMatrix() const;

        void markDirty();
        bool isDirty() const { return mIsDirty; }

        bool isRoot() const { return mParent.expired(); }

    private:
        friend class Scene;
        void removeChild(GraphNode* child);


    private:
        entt::entity mEntityId;

        bool mIsActive = true;
        std::string mTag;

        // Local transform data
        glm::vec3 mPosition{0.0f};
        glm::quat mRotation{1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 mScale{1.0f};

        mutable bool mIsDirty = true;
        mutable glm::mat4 mLocalMatrix{1.0f};
        mutable glm::mat4 mWorldMatrix{1.0f};

        std::weak_ptr<GraphNode> mParent;
        std::vector<std::shared_ptr<GraphNode>> mChildren;
    };

} // namespace StrikeEngine

#include "GraphNode.h"
#include "Scene.h"
#include <algorithm>

namespace StrikeEngine {
    GraphNode::GraphNode(Scene* scene, entt::entity handle, const std::string& id, const std::string& name)
        : m_scene(scene), m_entityHandle(handle), m_id(id), m_name(name), m_isDirty(true) {
    }

    void GraphNode::SetPosition(const glm::vec3& position) {
        m_localPosition = position;
        MarkDirty();
    }

    void GraphNode::SetRotation(const glm::vec3& eulerAngles) {
        glm::vec3 radians = glm::radians(eulerAngles);
        m_localRotation = glm::quat(radians);
        MarkDirty();
    }

    void GraphNode::SetRotation(const glm::quat& rotation) {
        m_localRotation = rotation;
        MarkDirty();
    }

    void GraphNode::SetScale(const glm::vec3& scale) {
        m_localScale = scale;
        MarkDirty();
    }

    glm::mat4 GraphNode::GetLocalMatrix() const {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_localPosition);
        glm::mat4 rotation = glm::mat4_cast(m_localRotation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_localScale);
        return translation * rotation * scale;
    }

    glm::mat4 GraphNode::GetWorldMatrix() const {
        if (m_isDirty) {
            if (m_parent != entt::null) {
                auto parentEntity = m_scene->GetEntityByHandle(m_parent);
                if (parentEntity) {
                    m_cachedWorldMatrix = parentEntity->GetWorldMatrix() * GetLocalMatrix();
                }
                else {
                    m_cachedWorldMatrix = GetLocalMatrix();
                }
            }
            else {
                m_cachedWorldMatrix = GetLocalMatrix();
            }
            m_isDirty = false;
        }
        return m_cachedWorldMatrix;
    }

    glm::vec3 GraphNode::GetWorldPosition() const {
        glm::mat4 worldMatrix = GetWorldMatrix();
        return glm::vec3(worldMatrix[3]);
    }

    glm::quat GraphNode::GetWorldRotation() const {
        if (m_parent != entt::null) {
            auto parentEntity = m_scene->GetEntityByHandle(m_parent);
            if (parentEntity) {
                return parentEntity->GetWorldRotation() * m_localRotation;
            }
        }
        return m_localRotation;
    }

    glm::vec3 GraphNode::GetWorldScale() const {
        if (m_parent != entt::null) {
            auto parentEntity = m_scene->GetEntityByHandle(m_parent);
            if (parentEntity) {
                return parentEntity->GetWorldScale() * m_localScale;
            }
        }
        return m_localScale;
    }

    void GraphNode::AddChild(entt::entity childId) {
        if (childId == entt::null) return;
        auto childEntity = m_scene->GetEntityByHandle(childId);
        if (childEntity) {
            m_children.push_back(childId);
        }
    }

    void GraphNode::RemoveChild(entt::entity childId) {
        m_children.erase(std::remove(m_children.begin(), m_children.end(), childId), m_children.end());
    }

    void GraphNode::SetParent(entt::entity parentId) {
        if (m_parent != entt::null) {
            auto oldParentEntity = m_scene->GetEntityByHandle(m_parent);
            if (oldParentEntity) {
                oldParentEntity->RemoveChild(m_entityHandle);
            }
        }

        m_parent = parentId;

        if (parentId != entt::null) {
            auto newParentEntity = m_scene->GetEntityByHandle(parentId);
            if (newParentEntity) {
                newParentEntity->AddChild(m_entityHandle);
            }
        }

        MarkDirty();
    }

    void GraphNode::MarkDirty() {
        m_isDirty = true;
        for (auto childId : m_children) {
            auto childEntity = m_scene->GetEntityByHandle(childId);
            if (childEntity) {
                childEntity->MarkDirty();
            }
        }
    }
}
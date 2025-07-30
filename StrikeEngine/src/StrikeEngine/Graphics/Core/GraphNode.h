#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <entt/entt.hpp>
#include <string>

namespace StrikeEngine {
    class Scene; // Forward declaration

    class GraphNode {
    public:
        GraphNode(Scene* scene, entt::entity handle, const std::string& id, const std::string& name = "");
        virtual ~GraphNode() = default;

        const std::string& GetId() const { return m_id; }
        const std::string& GetName() const { return m_name; }
        void SetName(const std::string& name) { m_name = name; }

        // Transform operations
        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::vec3& eulerAngles);
        void SetRotation(const glm::quat& rotation);
        void SetScale(const glm::vec3& scale);

        const glm::vec3& GetPosition() const { return m_localPosition; }
        const glm::quat& GetRotation() const { return m_localRotation; }
        const glm::vec3& GetScale() const { return m_localScale; }

        // World transform
        glm::mat4 GetWorldMatrix() const;
        glm::vec3 GetWorldPosition() const;
        glm::quat GetWorldRotation() const;
        glm::vec3 GetWorldScale() const;

        // Hierarchy management
        void AddChild(entt::entity childId);
        void RemoveChild(entt::entity childId);
        void SetParent(entt::entity parentId);

        entt::entity GetParent() const { return m_parent; }
        const std::vector<entt::entity>& GetChildren() const { return m_children; }

        // Matrix operations
        glm::mat4 GetLocalMatrix() const;
        void MarkDirty();

        entt::entity GetEntityHandle() const { return m_entityHandle; }

    protected:
        Scene* m_scene;
        entt::entity m_entityHandle;
        entt::entity m_parent = entt::null;
        std::vector<entt::entity> m_children;

        glm::vec3 m_localPosition{ 0.0f };
        glm::quat m_localRotation{ 1.0f, 0.0f, 0.0f, 0.0f };
        glm::vec3 m_localScale{ 1.0f };

        mutable bool m_isDirty = true;
        mutable glm::mat4 m_cachedWorldMatrix{ 1.0f };

        std::string m_id;
        std::string m_name;
    };
}
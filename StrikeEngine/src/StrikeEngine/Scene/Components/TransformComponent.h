#pragma once
#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <pugixml.hpp>


namespace StrikeEngine {

    class TransformComponent : public Component {
    public:
        TransformComponent() = default;
       
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "transform";
            return typeName;
        }

        const std::string& getTypeName() const override { return getStaticTypeName(); }

        bool isDirty() const { return mIsDirty; }

        // Position
        const glm::vec3& getPosition() const { return mPosition; }
        void setPosition(const glm::vec3& position) { mPosition = position; markDirty(); }
        void move(const glm::vec3& offset) { mPosition += offset; markDirty(); }

        // Rotation
        const glm::quat& getRotation() const { return mRotation; }
        void setRotation(const glm::quat& rotation) { mRotation = rotation; markDirty(); }
        void setRotationEuler(const glm::vec3& eulerAngles);
        void rotateEuler(const glm::vec3& anglesDegrees);
        
        void rotateQuaternion(float angleDegrees, glm::vec3 axis);
        void rotateX(float angleDegrees) { rotateQuaternion(angleDegrees, glm::vec3(1.0f, 0.0f, 0.0f)); }
        void rotateY(float angleDegrees) { rotateQuaternion(angleDegrees, glm::vec3(0.0f, 1.0f, 0.0f)); }
        void rotateZ(float angleDegrees) { rotateQuaternion(angleDegrees, glm::vec3(0.0f, 0.0f, 1.0f)); }
        glm::vec3 getRotationEuler() const;

        // Scale
        const glm::vec3& getScale() const { return mScale; }
        void setScale(const glm::vec3& scale) { mScale = scale; markDirty(); }
        void scale(const glm::vec3& factor) { mScale *= factor; markDirty(); }

        // Matrix operations
        const glm::mat4& getLocalMatrix() const;
        const glm::mat4& getWorldMatrix() const { return mWorldMatrix; }
        void setWorldMatrix(const glm::mat4& worldMatrix) const { mWorldMatrix = worldMatrix; }

        // Serialization
        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        void markDirty() const { mIsDirty = true; }
    private:

        void updateLocalMatrix() const;

    private:
        glm::vec3 mPosition{0.0f};
        glm::quat mRotation{1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 mScale{1.0f};
        mutable bool mIsDirty = true;
        mutable glm::mat4 mLocalMatrix{1.0f};
        mutable glm::mat4 mWorldMatrix{1.0f};
    };

} // namespace StrikeEngine
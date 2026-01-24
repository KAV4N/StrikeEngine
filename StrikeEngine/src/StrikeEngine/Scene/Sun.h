#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace StrikeEngine {

    class CameraComponent;

    class Sun {
    public:
        Sun() = default;
        ~Sun() = default;

        // Color (0–255)
        void setColor(const glm::uvec3& color);
        const glm::uvec3& getColor() const;

        // Intensity
        void setIntensity(float intensity);
        float getIntensity() const;

        // Rotation
        void setRotation(const glm::quat& rotation);
        const glm::quat& getRotation() const;

        void setRotationEuler(const glm::vec3& eulerDegrees);
        glm::vec3 getRotationEuler() const;

        void rotate(const glm::vec3& eulerDegrees);
        void rotateQuaternion(float angleDegrees, const glm::vec3& axis);

        // Direction (world space)
        glm::vec3 getDirection() const;

        // Shadows
        void setCastShadows(bool cast);
        bool getCastShadows() const;

        // Shadow mapping
        glm::mat4 calculateLightSpaceMatrix(const CameraComponent& camera);

    private:
        static constexpr float MAX_SHADOW_DISTANCE = 100.0f;

        glm::uvec3 mColor = glm::uvec3(255);
        float mIntensity = 1.0f;

        glm::quat mRotation = glm::quat(1, 0, 0, 0);
        bool mCastShadows = false;
    };

} // namespace StrikeEngine

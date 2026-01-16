#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

namespace StrikeEngine {

    class Sun {
    public:
        struct Frustum {
            std::array<glm::vec4, 6> planes;
        };

        Sun() = default;
        ~Sun() = default;

        void setColor(const glm::vec3& color);
        const glm::vec3& getColor() const;

        void setIntensity(float intensity);
        float getIntensity() const;

        void setRotationEuler(const glm::vec3& eulerAngles);
        void setRotation(const glm::quat& rotation);

        void rotateQuaternion(float angleDegrees, glm::vec3 axis);
        void rotateX(float angleDegrees);
        void rotateY(float angleDegrees);
        void rotateZ(float angleDegrees);

        glm::vec3 getRotationEuler() const;
        const glm::quat& getRotation() const { return mRotation; }
        glm::vec3 getDirection() const;

        void setCastShadows(bool cast);
        bool getCastShadows() const;

        // Updated to require camera direction
        glm::mat4 calculateLightSpaceMatrix(const glm::vec3& cameraPos, const glm::vec3& cameraDirection);
        const Frustum& getFrustum() const;

        void updateFrustum(const glm::vec3& cameraPos, const glm::vec3& cameraDirection);

    private:
        void extractFrustumPlanes(const glm::mat4& matrix);
        glm::vec3 normalizeEulerAngles(const glm::vec3& angles) const;

    private:
        glm::vec3 mColor = glm::vec3(1.0f);
        float mIntensity = 1.0f;

        // Sun has its own rotation (no position/scale needed)
        glm::quat mRotation{1.0f, 0.0f, 0.0f, 0.0f};

        bool mCastShadows = false;
        float mShadowDistance = 100.0f;
        float mShadowArea = 30.0f;

        glm::mat4 mLightSpaceMatrix{1.0f};
        Frustum mFrustum;
    };

} // namespace StrikeEngine
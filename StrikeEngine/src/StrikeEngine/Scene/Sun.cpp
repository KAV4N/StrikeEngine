#include "strikepch.h"
#include "Sun.h"
#include <glm/gtx/euler_angles.hpp>

namespace StrikeEngine {
    
    void Sun::setColor(const glm::vec3& color) {
        mColor = glm::clamp(color, glm::vec3(0.0f), glm::vec3(255.0f));
    }

    const glm::vec3& Sun::getColor() const {
        return mColor;
    }

    void Sun::setIntensity(float intensity) {
        mIntensity = intensity;
    }

    float Sun::getIntensity() const {
        return mIntensity;
    }

    void Sun::setRotationEuler(const glm::vec3& eulerAngles) {
        glm::vec3 normalized = normalizeEulerAngles(eulerAngles);
        glm::vec3 eulerRad = glm::radians(normalized);
        mRotation = glm::normalize(glm::quat(eulerRad));
    }

    void Sun::setRotation(const glm::quat& rotation) {
        mRotation = rotation;
    }

    void Sun::rotateX(float angleDegrees) {
        rotateQuaternion(angleDegrees, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    void Sun::rotateY(float angleDegrees) {
        rotateQuaternion(angleDegrees, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void Sun::rotateZ(float angleDegrees) {
        rotateQuaternion(angleDegrees, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void Sun::rotateQuaternion(float angleDegrees, glm::vec3 axis) {
        glm::quat angleQuat = glm::angleAxis(glm::radians(angleDegrees), axis);
        if (axis.y == 1.0f) {
            mRotation = angleQuat * mRotation;
        } else {
            mRotation = mRotation * angleQuat;
        }
    }

    glm::vec3 Sun::getRotationEuler() const {
        glm::vec3 eulerRad = glm::eulerAngles(mRotation);
        return glm::degrees(eulerRad);
    }

    glm::vec3 Sun::getDirection() const {
        return mRotation * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 Sun::normalizeEulerAngles(const glm::vec3& angles) const {
        auto normalizeAngle = [](float angle) {
            angle = fmod(angle + 180.0f, 360.0f);
            if (angle < 0.0f) angle += 360.0f;
            return angle - 180.0f;
        };
        
        return glm::vec3(
            normalizeAngle(angles.x),
            normalizeAngle(angles.y),
            normalizeAngle(angles.z)
        );
    }

    void Sun::setCastShadows(bool cast) {
        mCastShadows = cast;
    }

    bool Sun::getCastShadows() const {
        return mCastShadows;
    }


    glm::mat4 Sun::calculateLightSpaceMatrix(const glm::vec3& cameraPos, const glm::vec3& cameraDirection) {
         glm::vec3 sunDirection = getDirection();
        
        glm::vec3 camDir = glm::normalize(cameraDirection);
        float frontBias = 0.25f;
        glm::vec3 frustumCenter = cameraPos + camDir * (mShadowDistance * frontBias);
        glm::vec3 lightPos = frustumCenter - sunDirection * (mShadowDistance * 0.5f);
        
        glm::vec3 targetPos = frustumCenter;

        glm::mat4 lightProjection = glm::ortho(
            -mShadowArea, mShadowArea,
            -mShadowArea, mShadowArea,
            0.1f, mShadowDistance
        );

        glm::vec3 up = glm::abs(sunDirection.y) > 0.99f ? 
            glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 lightView = glm::lookAt(lightPos, targetPos, up);

        mLightSpaceMatrix = lightProjection * lightView;
        extractFrustumPlanes(mLightSpaceMatrix);
        return mLightSpaceMatrix;
    }

    const Sun::Frustum& Sun::getFrustum() const {
        return mFrustum;
    }

    void Sun::updateFrustum(const glm::vec3& cameraPos, const glm::vec3& cameraDirection) {
       
    }

    void Sun::extractFrustumPlanes(const glm::mat4& matrix) {
        // Left plane
        mFrustum.planes[0] = glm::vec4(
            matrix[0][3] + matrix[0][0],
            matrix[1][3] + matrix[1][0],
            matrix[2][3] + matrix[2][0],
            matrix[3][3] + matrix[3][0]
        );
        // Right plane
        mFrustum.planes[1] = glm::vec4(
            matrix[0][3] - matrix[0][0],
            matrix[1][3] - matrix[1][0],
            matrix[2][3] - matrix[2][0],
            matrix[3][3] - matrix[3][0]
        );
        // Bottom plane
        mFrustum.planes[2] = glm::vec4(
            matrix[0][3] + matrix[0][1],
            matrix[1][3] + matrix[1][1],
            matrix[2][3] + matrix[2][1],
            matrix[3][3] + matrix[3][1]
        );
        // Top plane
        mFrustum.planes[3] = glm::vec4(
            matrix[0][3] - matrix[0][1],
            matrix[1][3] - matrix[1][1],
            matrix[2][3] - matrix[2][1],
            matrix[3][3] - matrix[3][1]
        );
        // Near plane
        mFrustum.planes[4] = glm::vec4(
            matrix[0][3] + matrix[0][2],
            matrix[1][3] + matrix[1][2],
            matrix[2][3] + matrix[2][2],
            matrix[3][3] + matrix[3][2]
        );
        // Far plane
        mFrustum.planes[5] = glm::vec4(
            matrix[0][3] - matrix[0][2],
            matrix[1][3] - matrix[1][2],
            matrix[2][3] - matrix[2][2],
            matrix[3][3] - matrix[3][2]
        );

        // Normalize all planes
        for (auto& plane : mFrustum.planes) {
            float length = glm::length(glm::vec3(plane));
            if (length > 0.0f) plane /= length;
        }
    }

} // namespace StrikeEngine
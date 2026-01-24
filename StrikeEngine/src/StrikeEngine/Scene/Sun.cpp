#include "strikepch.h"
#include "Sun.h"
#include "StrikeEngine/Scene/Components/CameraComponent.h"
#include <glm/gtx/euler_angles.hpp>

namespace StrikeEngine {
    
    void Sun::setColor(const glm::uvec3& color) {
        mColor = glm::clamp(color, glm::uvec3(0.0f), glm::uvec3(255.0f));
    }

    const glm::uvec3& Sun::getColor() const {
        return mColor;
    }

    void Sun::setIntensity(float intensity) {
        mIntensity = glm::max(intensity, 0.0f);
    }

    float Sun::getIntensity() const {
        return mIntensity;
    }

    void Sun::setRotation(const glm::quat& rotation) {
        mRotation = glm::normalize(rotation);
    }

    void Sun::rotate(const glm::vec3& angles) {
        rotateQuaternion(angles.x, glm::vec3(1.0f, 0.0f, 0.0f));
        rotateQuaternion(angles.y, glm::vec3(0.0f, 1.0f, 0.0f));
        rotateQuaternion(angles.z, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void Sun::setRotationEuler(const glm::vec3& eulerAngles) {
        mRotation = glm::normalize(glm::quat(glm::radians(eulerAngles)));
    }

    void Sun::rotateQuaternion(float angleDegrees, const glm::vec3& axis) {
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
        glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
        return glm::normalize(mRotation * forward);
    }

    void Sun::setCastShadows(bool cast) {
        mCastShadows = cast;
    }

    bool Sun::getCastShadows() const {
        return mCastShadows;
    }

    glm::mat4 Sun::calculateLightSpaceMatrix(const CameraComponent& camera) {
        std::vector<glm::vec4> ndcCorners = {
            {-1, -1, -1, 1}, {1, -1, -1, 1},
            {-1,  1, -1, 1}, {1,  1, -1, 1},
            {-1, -1,  1, 1}, {1, -1,  1, 1},
            {-1,  1,  1, 1}, {1,  1,  1, 1}
        };

        glm::mat4 cameraProjection = camera.getProjectionMatrix();
        glm::mat4 cameraView = camera.getViewMatrix();

        glm::mat4 invViewProj = glm::inverse(cameraProjection * cameraView);
        std::vector<glm::vec3> worldCorners;
        worldCorners.reserve(8);

        for (const auto& corner : ndcCorners) {
            glm::vec4 worldPos = invViewProj * corner;
            worldCorners.push_back(glm::vec3(worldPos) / worldPos.w);
        }

        glm::vec3 center = glm::vec3(0.0f);
        for (const auto& corner : worldCorners) {
            center += corner;
        }
        center /= worldCorners.size();
        
        glm::mat4 lightView = glm::lookAt(
            center - glm::normalize(getDirection()),  // Light position
            center,                              // Look at frustum center
            glm::vec3(0, 1, 0)                  // Up vector
        );

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();
        
        for (const auto& corner : worldCorners) {
            glm::vec4 lightSpacePos = lightView * glm::vec4(corner, 1.0f);
            minX = glm::min(minX, lightSpacePos.x);
            maxX = glm::max(maxX, lightSpacePos.x);
            minY = glm::min(minY, lightSpacePos.y);
            maxY = glm::max(maxY, lightSpacePos.y);
            minZ = glm::min(minZ, lightSpacePos.z);
            maxZ = glm::max(maxZ, lightSpacePos.z);
        }

        constexpr float zMultiplier = 10.0f;
        if (minZ < 0) {
            minZ *= zMultiplier;
        } else {
            minZ /= zMultiplier;
        }

        glm::mat4 lightProjection = glm::ortho(
            minX, maxX,
            minY, maxY,
            minZ, maxZ
        );

        
        return lightProjection * lightView;;
    }

} // namespace StrikeEngine
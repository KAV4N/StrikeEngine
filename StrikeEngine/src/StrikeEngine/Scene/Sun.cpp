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
        glm::vec3 normAxis = glm::normalize(axis);

        glm::quat angleQuat = glm::angleAxis(
            glm::radians(angleDegrees),
            normAxis
        );

        if (normAxis.y == 1.0f)
            mRotation = angleQuat * mRotation;
        else
            mRotation = mRotation * angleQuat;

        mRotation = glm::normalize(mRotation);  

    }

    glm::vec3 Sun::getRotationEuler() const {
        glm::vec3 eulerRad = glm::eulerAngles(mRotation);
        return glm::degrees(eulerRad);
    }

    glm::vec3 Sun::getDirection() const {
        glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
        return mRotation * forward;
    }

    void Sun::setCastShadows(bool cast) {
        mCastShadows = cast;
    }

    bool Sun::getCastShadows() const {
        return mCastShadows;
    }

    glm::mat4 Sun::calculateLightSpaceMatrix(const CameraComponent& camera) {
        float shadowFar = glm::min(MAX_SHADOW_DISTANCE, camera.getFarPlane());
        float shadowNear = camera.getNearPlane();
        float fovY = glm::radians(camera.getFOV());
        float aspect = camera.getAspectRatio();

        float tanHalfFov = glm::tan(fovY * 0.5f);
        float nearH = shadowNear * tanHalfFov;
        float nearW = nearH * aspect;
        float farH  = shadowFar  * tanHalfFov;
        float farW  = farH  * aspect;

        glm::mat4 invView = glm::inverse(camera.getViewMatrix());

        glm::vec3 viewCorners[8] = {
            // Near plane
            { -nearW,  nearH, -shadowNear },
            {  nearW,  nearH, -shadowNear },
            { -nearW, -nearH, -shadowNear },
            {  nearW, -nearH, -shadowNear },
            // Far (shadow) plane
            { -farW,   farH, -shadowFar  },
            {  farW,   farH, -shadowFar  },
            { -farW,  -farH, -shadowFar  },
            {  farW,  -farH, -shadowFar  },
        };


        glm::vec3 worldCorners[8];
        glm::vec3 center = glm::vec3(0.0f);
        for (int i = 0; i < 8; ++i) {
            glm::vec4 w = invView * glm::vec4(viewCorners[i], 1.0f);
            worldCorners[i] = glm::vec3(w);
            center += worldCorners[i];
        }
        center /= 8.0f;

        glm::vec3 lightDir = getDirection();

        glm::vec3 up = (glm::abs(glm::dot(lightDir, glm::vec3(0, 1, 0))) > 0.99f)
                    ? glm::vec3(0, 0, 1)
                    : glm::vec3(0, 1, 0);

        glm::mat4 lightView = glm::lookAt(center - lightDir, center, up);

        float minX = std::numeric_limits<float>::max(),  maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max(),  maxY = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max(),  maxZ = std::numeric_limits<float>::lowest();

        for (const auto& corner : worldCorners) {
            glm::vec4 ls = lightView * glm::vec4(corner, 1.0f);
            minX = glm::min(minX, ls.x); maxX = glm::max(maxX, ls.x);
            minY = glm::min(minY, ls.y); maxY = glm::max(maxY, ls.y);
            minZ = glm::min(minZ, ls.z); maxZ = glm::max(maxZ, ls.z);
        }

        constexpr float zPullback = 10.0f;
        minZ -= zPullback;

        glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
        return lightProjection * lightView;
    }
} 
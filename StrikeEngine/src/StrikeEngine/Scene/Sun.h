#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Strike {

    class CameraComponent;

    /**
     * @brief Represents the sun in the scene, providing directional light.
     */
    class Sun {
    public:
        Sun() = default;
        ~Sun() = default;

        /**
         * @brief set the color of the sunlight.
         * @param color RGB color as glm::uvec3 (0-255 range).
         */
        void setColor(const glm::uvec3& color);

        /**
         * @brief Get the color of the sunlight.
         * @return RGB color as glm::uvec3 (0-255 range).
         */
        const glm::uvec3& getColor() const;

        /**
         * @brief Set the intensity of the sunlight.
         * @param intensity Intensity value (1.0 = normal brightness).
         */
        void setIntensity(float intensity);

        /**
         * @brief Get the intensity of the sunlight.
         * @return Intensity value.
         */
        float getIntensity() const;

        /**
         * @brief Set the rotation of the sun using a quaternion.
         * @param rotation Quaternion representing the rotation.
         */
        void setRotation(const glm::quat& rotation);

        /**
         * @brief Get the rotation of the sun as a quaternion.
         * @return Quaternion representing the rotation.
         */
        const glm::quat& getRotation() const;

        /**
         * @brief Set the rotation of the sun using Euler angles in degrees.
         * @param eulerDegrees Euler angles in degrees (x,y,z).
         */
        void setRotationEuler(const glm::vec3& eulerDegrees);

        /**
         * @brief Get the rotation of the sun as Euler angles in degrees.
         * @return Euler angles in degrees (x,y,z).
         */
        glm::vec3 getRotationEuler() const;

        /**
         * @brief Rotate the sun by given Euler angles in degrees.
         * @param eulerDegrees Euler angles in degrees (x,y,z).
         */
        void rotate(const glm::vec3& eulerDegrees);

        /**
         * @brief Rotate the sun around a specified axis by a given angle in degrees.
         * @param angleDegrees Angle in degrees.
         * @param axis Axis to rotate around.
         */
        void rotateQuaternion(float angleDegrees, const glm::vec3& axis);

        /**
         * @brief Get the direction vector of the sunlight.
         * @return Normalized direction vector.
         */
        glm::vec3 getDirection() const;

        /**
         * @brief Set whether the sun casts shadows.
         * @param cast True to enable shadow casting, false to disable.
         */
        void setCastShadows(bool cast);
        
        /**
         * @brief Check if the sun casts shadows.
         * @return True if shadow casting is enabled, false otherwise.
         */
        bool getCastShadows() const;

        /**
         * @brief Calculate the light space matrix for shadow mapping.
         * 
         * @param camera The camera component to base the calculation on.
         * @return Light space matrix (projection * view) for shadow mapping.
         */
        glm::mat4 calculateLightSpaceMatrix(const CameraComponent& camera);

    private:
        static constexpr float MAX_SHADOW_DISTANCE = 40.0f;

        glm::uvec3 mColor = glm::uvec3(255);
        float mIntensity = 1.0f;

        glm::quat mRotation = glm::quat(1, 0, 0, 0);
        bool mCastShadows = false;
    };

}
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "StrikeEngine/Scene/Components/TransformComponents.h"

namespace StrikeEngine {
    struct FrustumCluster {
        float nearDepth;
        float farDepth;
        glm::vec3 minBounds;
        glm::vec3 maxBounds;
    };

    struct CameraComponent {



        glm::vec3 Right;
        glm::vec3 Up;
        glm::vec3 Forward;

        glm::mat4 ProjectionMatrix;
        glm::mat4 ViewMatrix;

        const glm::vec3 WorldUp = glm::vec3(0, 1, 0);




        float fov;
        float aspectRatio;
        float nearPlane;
        float farPlane;
        std::vector<FrustumCluster> clusters;

        CameraComponent(float fov, float aspectRatio, float nearPlane, float farPlane, int numClusters = 4)
            : fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane)
        {
            ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
            UpdateViewMatrix(glm::vec3(0.0f), glm::vec3(0.0f));
            CreateClusters(numClusters);
        }

        void UpdateViewMatrix(const glm::vec3& position, const glm::vec3& rotation) {
            /*
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
            transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1, 0, 0));
            transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0, 1, 0));
            transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0, 0, 1));
            ViewMatrix = glm::inverse(transform);
            */


            Forward = glm::normalize(glm::vec3(
                cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x)),
                sin(glm::radians(rotation.x)),
                sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x))
            ));
            Right = glm::normalize(glm::cross(Forward, WorldUp));
            Up = glm::normalize(glm::cross(Right, Forward));

            ViewMatrix = glm::lookAt(position, position + Forward, Up);
        }

        glm::vec3 GetForwardDirection() const {
            return Forward;
        }

        glm::vec3 GetRightDirection() const {
            return Right;
        }

        glm::vec3 GetUpDirection() const {
            return Up;
        }

        void CreateClusters(int numClusters) {
            clusters.clear();
            clusters.reserve(numClusters);

            float ratio = std::pow(farPlane / nearPlane, 1.0f / numClusters);
            float nearDepth = nearPlane;

            for (int i = 0; i < numClusters; ++i) {
                float farDepth = nearDepth * ratio;
                clusters.push_back({ nearDepth, farDepth });
                nearDepth = farDepth;
            }
        }

        void CalculateClusterBounds(FrustumCluster& cluster, const glm::vec3& position) {
            float nearHeight = 2 * std::tan(glm::radians(fov * 0.5f)) * cluster.nearDepth;
            float nearWidth = nearHeight * aspectRatio;
            float farHeight = 2 * std::tan(glm::radians(fov * 0.5f)) * cluster.farDepth;
            float farWidth = farHeight * aspectRatio;

            glm::vec3 nearCenter = position + Forward * cluster.nearDepth;
            glm::vec3 farCenter = position + Forward * cluster.farDepth;

            cluster.minBounds = glm::min(
                nearCenter - Right * (nearWidth * 0.5f) - Up * (nearHeight * 0.5f),
                farCenter - Right * (farWidth * 0.5f) - Up * (farHeight * 0.5f)
            );

            cluster.maxBounds = glm::max(
                nearCenter + Right * (nearWidth * 0.5f) + Up * (nearHeight * 0.5f),
                farCenter + Right * (farWidth * 0.5f) + Up * (farHeight * 0.5f)
            );
        }

        void UpdateClusters(const glm::vec3& position) {
            for (auto& cluster : clusters) {
                CalculateClusterBounds(cluster, position);
            }
        }
    };
}
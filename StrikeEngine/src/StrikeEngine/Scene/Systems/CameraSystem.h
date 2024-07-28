#pragma once
#include <entt/entt.hpp>
#include "StrikeEngine/Scene/Components/CameraComponent.h"

namespace StrikeEngine {
    class CameraSystem {
    public:
        static void SetPosition(entt::registry& registry, entt::entity entity, const glm::vec3& position) {
            auto& camera = registry.get<CameraComponent>(entity);
            camera.Position = position;
            camera.UpdateViewMatrix();
        }

        static void SetRotation(entt::registry& registry, entt::entity entity, const glm::vec3& rotation) {
            auto& camera = registry.get<CameraComponent>(entity);
            camera.Rotation = rotation;
            camera.UpdateViewMatrix();
        }

        static void MoveForward(entt::registry& registry, entt::entity entity, float distance) {
            auto& camera = registry.get<CameraComponent>(entity);
            camera.Position += camera.GetForwardDirection() * distance;
            camera.UpdateViewMatrix();
        }

        static void MoveBackward(entt::registry& registry, entt::entity entity, float distance) {
            auto& camera = registry.get<CameraComponent>(entity);
            camera.Position -= camera.GetForwardDirection() * distance;
            camera.UpdateViewMatrix();
        }

        static void MoveLeft(entt::registry& registry, entt::entity entity, float distance) {
            auto& camera = registry.get<CameraComponent>(entity);
            camera.Position -= camera.GetRightDirection() * distance;
            camera.UpdateViewMatrix();
        }

        static void MoveRight(entt::registry& registry, entt::entity entity, float distance) {
            auto& camera = registry.get<CameraComponent>(entity);
            camera.Position += camera.GetRightDirection() * distance;
            camera.UpdateViewMatrix();
        }

        static void RotateYaw(entt::registry& registry, entt::entity entity, float angle) {
            auto& camera = registry.get<CameraComponent>(entity);
            camera.Rotation.y += angle;
            camera.UpdateViewMatrix();
        }

        static void RotatePitch(entt::registry& registry, entt::entity entity, float angle) {
            auto& camera = registry.get<CameraComponent>(entity);
            camera.Rotation.x += angle;
            camera.UpdateViewMatrix();
        }
    };
}

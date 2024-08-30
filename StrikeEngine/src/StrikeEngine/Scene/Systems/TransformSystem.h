#pragma once

#include <entt/entt.hpp>
#include "StrikeEngine/Scene/Components/TransformComponents.h"
#include "StrikeEngine/Scene/Components/ModelComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include "StrikeEngine/Scene/Entity.h"

namespace StrikeEngine {
    class TransformSystem {
    public:

        static void Update(Scene* scene) {
            auto view = scene->GetRegistry().view<PositionComponent, RotationComponent, ScaleComponent, TransformComponent, ModelComponent>();

            for (auto entityHandle : view) {
                Entity entity(entityHandle, scene);
                UpdateTransformComponent(entity);

                auto& modelComp = entity.GetComponent<ModelComponent>();


                for (auto& part : modelComp.parts) {
                    UpdateTransformComponent(part);
                }
            }
        }

        // Setters for components
        static void SetPosition(Entity entity, const glm::vec3& position) {
            entity.GetComponent<PositionComponent>().position = position;
            UpdateTransformComponent(entity);
        }

        static void SetRotation(Entity entity, const glm::vec3& rotation) {
            entity.GetComponent<RotationComponent>().rotation = rotation;
            UpdateTransformComponent(entity);
        }

        static void SetScale(Entity entity, const glm::vec3& scale) {
            entity.GetComponent<ScaleComponent>().scale = scale;
            UpdateTransformComponent(entity);
        }

        static void IncreasePosition(Entity entity, const glm::vec3& delta) {
            entity.GetComponent<PositionComponent>().position += delta;
            UpdateTransformComponent(entity);
        }

        static void IncreaseRotation(Entity entity, const glm::vec3& delta) {
            entity.GetComponent<RotationComponent>().rotation += delta;
            UpdateTransformComponent(entity);
        }

        static void IncreaseScale(Entity entity, const glm::vec3& delta) {
            entity.GetComponent<ScaleComponent>().scale += delta;
            UpdateTransformComponent(entity);
        }




    private:

        static glm::mat4 CalculateTransformMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
                * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1))
                * glm::scale(glm::mat4(1.0f), scale);
            return transform;
        }


        static void UpdateTransformComponent(Entity entity) {
            const auto& position = entity.GetComponent<PositionComponent>().position;
            const auto& rotation = entity.GetComponent<RotationComponent>().rotation;
            const auto& scale = entity.GetComponent<ScaleComponent>().scale;
            auto& transform = entity.GetComponent<TransformComponent>().transformationMatrix;

            transform = CalculateTransformMatrix(position, rotation, scale);
        }
    };
}

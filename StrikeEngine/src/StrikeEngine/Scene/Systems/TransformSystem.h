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

                auto& position = entity.GetComponent<PositionComponent>().position;
                auto& rotation = entity.GetComponent<RotationComponent>().rotation;
                auto& scale = entity.GetComponent<ScaleComponent>().scale;
                auto& transform = entity.GetComponent<TransformComponent>().transformationMatrix;
                auto& modelComp = entity.GetComponent<ModelComponent>();

                glm::mat4 entityTransform = CalculateTransformMatrix(position, rotation, scale);

                transform = entityTransform;

                for (auto& part : modelComp.parts) {
                    part.UpdateLocalTransform();
                }
            }
        }

        static void SetPosition(Entity entity, const glm::vec3& position) {
            entity.GetComponent<PositionComponent>().position = position;
        }

        static void SetRotation(Entity entity, const glm::vec3& rotation) {
            entity.GetComponent<RotationComponent>().rotation = rotation;
        }

        static void SetScale(Entity entity, const glm::vec3& scale) {
            entity.GetComponent<ScaleComponent>().scale = scale;
        }

        static void IncreasePosition(Entity entity, const glm::vec3& delta) {
            entity.GetComponent<PositionComponent>().position += delta;
        }

        static void IncreaseRotation(Entity entity, const glm::vec3& delta) {
            entity.GetComponent<RotationComponent>().rotation += delta;
        }

        static void IncreaseScale(Entity entity, const glm::vec3& delta) {
            entity.GetComponent<ScaleComponent>().scale += delta;
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
    };
}

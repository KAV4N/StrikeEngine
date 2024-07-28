#pragma once

#include "StrikeEngine/Renderer/Model.h"
#include <glm/glm.hpp>
#include <vector>

namespace StrikeEngine {

    struct ModelPartComponent {
        ModelPart* part;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        glm::mat4 localTransform;

        ModelPartComponent(ModelPart* p)
            : part(p), position(0.0f), rotation(0.0f), scale(1.0f), localTransform(1.0f) {}

        void UpdateLocalTransform() {
            localTransform = glm::translate(glm::mat4(1.0f), position)
                * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0))
                * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1))
                * glm::scale(glm::mat4(1.0f), scale);
        }
    };

    struct ModelComponent {
        Model* model;
        std::vector<ModelPartComponent> parts;

        ModelComponent(Model* m) : model(m) {
            for (auto* part : m->GetParts()) {
                parts.emplace_back(part);
            }
        }
    };
}

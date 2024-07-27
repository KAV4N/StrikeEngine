#pragma once

#include "StrikeEngine/Renderer/Model.h"
#include "StrikeEngine/Renderer/Texture.h"
#include "StrikeEngine/Entity.h"

namespace StrikeEngine {
    class Baseplate : public Entity {
    public:
        Baseplate(Shader* shader, float width, float height, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f))
            : Entity(CreateModel(shader, width, height), position, rotation, scale) {}

    private:
        static Model* CreateModel(Shader* shader, float width, float height) {
            // Create vertices for the baseplate (a simple rectangle)
            float halfWidth = width / 2.0f;
            float halfHeight = height / 2.0f;

            std::vector<float> vertices = {
                // Positions          // Normals        // Texture Coords
                -halfWidth, 0.0f, -halfHeight, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                 halfWidth, 0.0f, -halfHeight, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                 halfWidth, 0.0f,  halfHeight, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                -halfWidth, 0.0f,  halfHeight, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            };

            std::vector<unsigned int> indices = {
                0, 1, 2,
                2, 3, 0
            };

            // Create and configure the model part
            unsigned int vaoID, vboID, eboID;
            glGenVertexArrays(1, &vaoID);
            glGenBuffers(1, &vboID);
            glGenBuffers(1, &eboID);

            glBindVertexArray(vaoID);

            glBindBuffer(GL_ARRAY_BUFFER, vboID);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            // Position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            // Normal attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            // Texture coordinate attribute
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);

            // Create the model part and model
            ModelPart* part = new ModelPart(vaoID, indices.size(), vboID, eboID);
            part->SetMaterial(DEFAULT_MATERIAL);

            Model* model = new Model(shader);
            model->AddPart(part);

            return model;
        }
    };
}

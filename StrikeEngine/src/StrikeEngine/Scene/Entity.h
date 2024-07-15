#pragma once

#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace StrikeEngine {
    class Entity {
    public:
        Entity(Model* model, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f))
            : m_Model(model), m_Position(position), m_Rotation(rotation), m_Scale(scale) {}


        inline void SetPosition(const glm::vec3& position) { m_Position = position; }
        inline void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }
        void SetScale(const glm::vec3& scale) { m_Scale = scale; }


        inline void IncreasePosition(const glm::vec3& delta) { m_Position += delta; }
        inline void IncreaseRotation(const glm::vec3& delta) { m_Rotation += delta; }
        inline void IncreaseScale(const glm::vec3& delta) { m_Scale += delta; }

  
        inline Model* GetModel() const { return m_Model; }
        inline glm::vec3 GetPosition() const { return m_Position; }
        inline glm::vec3 GetRotation() const { return m_Rotation; }
        inline glm::vec3 GetScale() const { return m_Scale; }

        glm::mat4 GetTransformationMatrix() const {
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_Position);
            glm::mat4 rotationXMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
            glm::mat4 rotationYMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
            glm::mat4 rotationZMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));
            glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), m_Scale);

            return translationMatrix * rotationXMatrix * rotationYMatrix * rotationZMatrix * scaleMatrix;
        }

    private:
        Model* m_Model;
        glm::vec3 m_Position;
        glm::vec3 m_Rotation;
        glm::vec3 m_Scale;
    };
}

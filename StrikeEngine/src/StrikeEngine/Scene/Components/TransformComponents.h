#pragma once

#include "StrikeEngine/Renderer/Core/Model.h"
#include <glm/glm.hpp>

namespace StrikeEngine {


    struct PositionComponent {
        glm::vec3 position;
        PositionComponent(glm::vec3 pos = glm::vec3(0.0f)) 
            : position(pos) 
        {}
    };

    struct RotationComponent {
        glm::vec3 rotation;
        RotationComponent(glm::vec3 rot = glm::vec3(0.0f)) 
            : rotation(rot) 
        {}
    };

    struct ScaleComponent {
        glm::vec3 scale;
        ScaleComponent(glm::vec3 sca= glm::vec3(1.0f)) 
            : scale(sca) 
        {}
    };

    struct TransformComponent {
        glm::mat4 transformationMatrix;
        TransformComponent(glm::mat4 transMat = glm::mat4(0.0f))
            : transformationMatrix(transMat)
        {

        }
    };
}

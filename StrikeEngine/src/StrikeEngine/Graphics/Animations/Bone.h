#pragma once
#include <glm/glm.hpp>
#include <string>

namespace StrikeEngine {
    struct Bone {
        glm::mat4 offsetMatrix;
        std::string name;
        int parentIndex;
    };
}
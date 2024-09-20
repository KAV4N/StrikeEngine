#include "strikepch.h"
#include "VisibilityCuller.h"

namespace StrikeEngine {

    /*
    bool VisibilityCuller::IsVisible(const ModelPart::AABB& aabb, const glm::mat4& modelMatrix, const glm::mat4& viewProjectionMatrix) {
        std::array<glm::vec4, 8> corners = {
            glm::vec4(aabb.min.x, aabb.min.y, aabb.min.z, 1.0f),
            glm::vec4(aabb.max.x, aabb.min.y, aabb.min.z, 1.0f),
            glm::vec4(aabb.min.x, aabb.max.y, aabb.min.z, 1.0f),
            glm::vec4(aabb.max.x, aabb.max.y, aabb.min.z, 1.0f),
            glm::vec4(aabb.min.x, aabb.min.y, aabb.max.z, 1.0f),
            glm::vec4(aabb.max.x, aabb.min.y, aabb.max.z, 1.0f),
            glm::vec4(aabb.min.x, aabb.max.y, aabb.max.z, 1.0f),
            glm::vec4(aabb.max.x, aabb.max.y, aabb.max.z, 1.0f)
        };

        for (auto& corner : corners) {
            corner = viewProjectionMatrix * modelMatrix * corner;
            corner /= corner.w;
        }

        bool visible = false;
        for (const auto& corner : corners) {
            if (corner.x >= -1.0f && corner.x <= 1.0f &&
                corner.y >= -1.0f && corner.y <= 1.0f &&
                corner.z >= -1.0f && corner.z <= 1.0f) {
                visible = true;
                break;
            }
        }

        return visible;
    }
    */
}
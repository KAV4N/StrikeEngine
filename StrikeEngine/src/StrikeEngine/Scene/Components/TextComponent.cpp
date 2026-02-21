#include "strikepch.h"
#include "TextComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

namespace Strike {

    REGISTER_COMPONENT(TextComponent)

    void TextComponent::deserialize(const pugi::xml_node& node) {
        if (auto attr = node.attribute("text")) {
            mText = attr.as_string();
        }

        if (auto attr = node.attribute("color")) {
            unsigned int r = 255, g = 255, b = 255;
            if (sscanf(attr.as_string(), "%u,%u,%u", &r, &g, &b) == 3) {
                setColor(glm::uvec3(r, g, b));
            }
        }


        if (auto attr = node.attribute("pivot")) {
            std::string pivotStr = attr.as_string();
            std::stringstream ss(pivotStr);
            char comma;
            glm::vec2 pivot;
            if (ss >> pivot.x >> comma >> pivot.y) {
                setPivot(pivot);
            }
        }

        if (auto attr = node.attribute("x")) {
            mPosition.x = glm::clamp(attr.as_float(0.0f), 0.0f, 1.0f);
        }

        if (auto attr = node.attribute("y")) {
            mPosition.y = glm::clamp(attr.as_float(0.0f), 0.0f, 1.0f);
        }

        if (auto attr = node.attribute("active")) {
            setActive(attr.as_bool(true));
        }
    }

} 
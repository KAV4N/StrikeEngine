#include "strikepch.h"
#include "TextComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

namespace StrikeEngine {

    REGISTER_COMPONENT(TextComponent)

    void TextComponent::deserialize(const pugi::xml_node& node) {
        if (auto attr = node.attribute("text")) {
            mText = attr.as_string();
        }

        if (auto attr = node.attribute("color")) {
            std::string colorStr = attr.as_string();
            std::stringstream ss(colorStr);
            char comma1, comma2;
            ss >> mColor.r >> comma1 >> mColor.g >> comma2 >> mColor.b;
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

        if (auto attr = node.attribute("active")) {
            setActive(attr.as_bool(true));
        }
    }

    void TextComponent::serialize(pugi::xml_node& node) const {
        node.append_attribute("text") = mText.c_str();


        std::stringstream colorStr;
        colorStr << static_cast<int>(mColor.r) << ","
                 << static_cast<int>(mColor.g) << ","
                 << static_cast<int>(mColor.b);
        node.append_attribute("color") = colorStr.str().c_str();


        std::stringstream pivotStr;
        pivotStr << std::fixed << std::setprecision(3) << mPivot.x << "," << mPivot.y;
        node.append_attribute("pivot") = pivotStr.str().c_str();

        node.append_attribute("active") = isActive();
    }

} // namespace StrikeEngine
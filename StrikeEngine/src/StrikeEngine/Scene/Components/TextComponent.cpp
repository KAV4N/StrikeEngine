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
            char comma;
            ss >> mColor.r >> comma >> mColor.g >> comma >> mColor.b;
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
        
        node.append_attribute("active") = isActive();
    }
}
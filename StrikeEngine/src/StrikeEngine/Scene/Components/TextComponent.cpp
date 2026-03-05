#include "strikepch.h"
#include "TextComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"
#include "StrikeEngine/Core/ParserUtils.h"

namespace Strike {

    REGISTER_COMPONENT(TextComponent)

    void TextComponent::deserialize(const pugi::xml_node& node) {
        if (auto attr = node.attribute("text"))
            mText = attr.as_string();

        if (auto attr = node.attribute("color"))
            setColor(ParserUtils::parseUVec3(attr.as_string(), glm::uvec3(255, 255, 255)));

        if (auto attr = node.attribute("pivot"))
            setPivot(ParserUtils::parseVec2(attr.as_string(), glm::vec2(0.0f)));

        if (auto attr = node.attribute("x"))
            mPosition.x = glm::clamp(attr.as_float(0.0f), 0.0f, 1.0f);

        if (auto attr = node.attribute("y"))
            mPosition.y = glm::clamp(attr.as_float(0.0f), 0.0f, 1.0f);

        if (auto attr = node.attribute("active"))
            setActive(attr.as_bool(true));
    }

}
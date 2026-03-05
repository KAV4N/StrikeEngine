#include "strikepch.h"
#include "LightComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"
#include "StrikeEngine/Core/ParserUtils.h"

namespace Strike {

    REGISTER_COMPONENT(LightComponent);

    LightComponent::LightComponent(const glm::uvec3& color, float intensity, float radius)
        : mColor(color), mIntensity(intensity), mRadius(radius)
    {
    }

    void LightComponent::deserialize(const pugi::xml_node& node) {
        if (auto attr = node.attribute("color"))
            setColor(ParserUtils::parseUVec3(attr.as_string(), glm::uvec3(255, 255, 255)));

        if (auto attr = node.attribute("intensity"))
            setIntensity(attr.as_float());

        if (auto attr = node.attribute("radius"))
            setRadius(attr.as_float());

        if (auto attr = node.attribute("active"))
            setActive(attr.as_bool(true));
    }
}
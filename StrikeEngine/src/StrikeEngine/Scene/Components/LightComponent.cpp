#include "strikepch.h"
#include "LightComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

namespace StrikeEngine {

    REGISTER_COMPONENT(LightComponent);

    LightComponent::LightComponent(const glm::vec3& color, float intensity, float radius)
        : mColor(color), mIntensity(intensity), mRadius(radius)
    {
    }

    void LightComponent::deserialize(const pugi::xml_node& node) {
        if (auto attr = node.attribute("color")) {
            glm::vec3 color;
            sscanf(attr.as_string(), "%f,%f,%f", &color.x, &color.y, &color.z);
            setColor(color);
        }
        if (auto attr = node.attribute("intensity")) {
            setIntensity(attr.as_float());
        }
        if (auto attr = node.attribute("radius")) {
            setRadius(attr.as_float());
        }

        if (auto attr = node.attribute("active")) {
            setActive(attr.as_bool(true));
        }
    }

    void LightComponent::serialize(pugi::xml_node& node) const {
        char colorStr[64];
        snprintf(colorStr, sizeof(colorStr), "%f,%f,%f", mColor.x, mColor.y, mColor.z);
        node.append_attribute("color")     = colorStr;
        node.append_attribute("intensity") = mIntensity;
        node.append_attribute("radius")    = mRadius;
        node.append_attribute("active")    = isActive();
    }

} // namespace StrikeEngine
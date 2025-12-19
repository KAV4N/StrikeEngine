#include "strikepch.h"
#include "LightComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"
#include <glm/gtc/type_ptr.hpp>

namespace StrikeEngine {
    REGISTER_COMPONENT(LightComponent);

    // PointLightComponent implementation
    LightComponent::LightComponent(const glm::vec3& color, float intensity, float radius, float fallOff)
        : mColor(color), mIntensity(intensity), mRadius(radius), mFallOff(fallOff) {
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
        if (auto attr = node.attribute("fallOff")) {
            setFallOff(attr.as_float());
        }
    }

    void LightComponent::serialize(pugi::xml_node& node) const {
        char colorStr[64];
        snprintf(colorStr, sizeof(colorStr), "%f,%f,%f", mColor.x, mColor.y, mColor.z);
        node.append_attribute("color") = colorStr;
        node.append_attribute("intensity") = mIntensity;
        node.append_attribute("radius") = mRadius;
        node.append_attribute("fallOff") = mFallOff;
    }
}
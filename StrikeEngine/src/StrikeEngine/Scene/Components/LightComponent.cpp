#include "strikepch.h"
#include "LightComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

#include <cstdio>   // snprintf, sscanf
#include <cstring>  // memset (optional)

namespace Strike {

    REGISTER_COMPONENT(LightComponent);

    LightComponent::LightComponent(const glm::uvec3& color, float intensity, float radius)
        : mColor(color), mIntensity(intensity), mRadius(radius)
    {
    }

    void LightComponent::deserialize(const pugi::xml_node& node) {
        if (auto attr = node.attribute("color")) {
            unsigned int r = 255, g = 255, b = 255;
            if (sscanf(attr.as_string(), "%u,%u,%u", &r, &g, &b) == 3) {
                setColor(glm::uvec3(r, g, b));
            }
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
} 
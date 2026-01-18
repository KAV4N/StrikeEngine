#include "strikepch.h"
#include "LightComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

#include <cstdio>   // snprintf, sscanf
#include <cstring>  // memset (optional)

namespace StrikeEngine {

    REGISTER_COMPONENT(LightComponent);

    LightComponent::LightComponent(const glm::uvec3& color, float intensity, float radius)
        : mColor(color), mIntensity(intensity), mRadius(radius)
    {
    }

    void LightComponent::deserialize(const pugi::xml_node& node) {
        if (auto attr = node.attribute("color")) {
            unsigned int r = 255, g = 255, b = 255;
            // Try both common formats: "255,128,0"  or  "1.0,0.5,0.0"
            if (sscanf(attr.as_string(), "%u,%u,%u", &r, &g, &b) == 3) {
                // integer format → perfect
                setColor(glm::uvec3(r, g, b));
            }
            else {
                float fr, fg, fb;
                if (sscanf(attr.as_string(), "%f,%f,%f", &fr, &fg, &fb) == 3) {
                    // float 0–1 → convert to 0–255
                    setColor(glm::uvec3(
                        glm::clamp(int(fr * 255.0f + 0.5f), 0, 255),
                        glm::clamp(int(fg * 255.0f + 0.5f), 0, 255),
                        glm::clamp(int(fb * 255.0f + 0.5f), 0, 255)
                    ));
                }
                // else → leave default (255,255,255)
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

    void LightComponent::serialize(pugi::xml_node& node) const {
        char colorStr[64];
        // Save as integers — most readable & editor-friendly
        snprintf(colorStr, sizeof(colorStr), "%u,%u,%u",
                 mColor.r, mColor.g, mColor.b);

        node.append_attribute("color")     = colorStr;
        node.append_attribute("intensity") = mIntensity;
        node.append_attribute("radius")    = mRadius;
        node.append_attribute("active")    = isActive();
    }

} // namespace StrikeEngine
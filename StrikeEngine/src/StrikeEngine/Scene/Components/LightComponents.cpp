#include "LightComponents.h"
#include <glm/gtc/type_ptr.hpp>

namespace StrikeEngine {

    // PointLightComponent implementation
    PointLightComponent::PointLightComponent(const glm::vec3& color, float intensity, float radius)
        : mRadius(radius) {
        setColor(color);
        setIntensity(intensity);
    }

    void PointLightComponent::deserialize(const pugi::xml_node& node) {
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
        if (auto attr = node.attribute("attenuation")) {
            glm::vec3 attenuation;
            sscanf(attr.as_string(), "%f,%f,%f", &attenuation.x, &attenuation.y, &attenuation.z);
            setAttenuation(attenuation);
        }
        if (auto attr = node.attribute("castShadows")) {
            setCastShadows(attr.as_bool());
        }
    }

    void PointLightComponent::serialize(pugi::xml_node& node) const {
        char colorStr[64];
        snprintf(colorStr, sizeof(colorStr), "%f,%f,%f", mColor.x, mColor.y, mColor.z);
        node.append_attribute("color") = colorStr;
        node.append_attribute("intensity") = mIntensity;
        node.append_attribute("radius") = mRadius;
        
        char attenStr[64];
        snprintf(attenStr, sizeof(attenStr), "%f,%f,%f", mAttenuation.x, mAttenuation.y, mAttenuation.z);
        node.append_attribute("attenuation") = attenStr;
        node.append_attribute("castShadows") = mCastShadows;
    }

    // DirectionalLightComponent implementation
    DirectionalLightComponent::DirectionalLightComponent(const glm::vec3& color, float intensity, 
                                                       const glm::vec3& direction) {
        setColor(color);
        setIntensity(intensity);
        setDirection(direction);
    }

    void DirectionalLightComponent::deserialize(const pugi::xml_node& node) {
        if (auto attr = node.attribute("color")) {
            glm::vec3 color;
            sscanf(attr.as_string(), "%f,%f,%f", &color.x, &color.y, &color.z);
            setColor(color);
        }
        if (auto attr = node.attribute("intensity")) {
            setIntensity(attr.as_float());
        }
        if (auto attr = node.attribute("direction")) {
            glm::vec3 direction;
            sscanf(attr.as_string(), "%f,%f,%f", &direction.x, &direction.y, &direction.z);
            setDirection(direction);
        }
        if (auto attr = node.attribute("castShadows")) {
            setCastShadows(attr.as_bool());
        }
    }

    void DirectionalLightComponent::serialize(pugi::xml_node& node) const {
        char colorStr[64];
        snprintf(colorStr, sizeof(colorStr), "%f,%f,%f", mColor.x, mColor.y, mColor.z);
        node.append_attribute("color") = colorStr;
        node.append_attribute("intensity") = mIntensity;
        
        char dirStr[64];
        snprintf(dirStr, sizeof(dirStr), "%f,%f,%f", mDirection.x, mDirection.y, mDirection.z);
        node.append_attribute("direction") = dirStr;
        node.append_attribute("castShadows") = mCastShadows;
    }

    // SpotLightComponent implementation
    SpotLightComponent::SpotLightComponent(const glm::vec3& color, float intensity, 
                                         const glm::vec3& direction, float innerConeAngle, 
                                         float outerConeAngle) 
        : mInnerConeAngle(innerConeAngle), mOuterConeAngle(outerConeAngle) {
        setColor(color);
        setIntensity(intensity);
        setDirection(direction);
    }

    void SpotLightComponent::deserialize(const pugi::xml_node& node) {
        if (auto attr = node.attribute("color")) {
            glm::vec3 color;
            sscanf(attr.as_string(), "%f,%f,%f", &color.x, &color.y, &color.z);
            setColor(color);
        }
        if (auto attr = node.attribute("intensity")) {
            setIntensity(attr.as_float());
        }
        if (auto attr = node.attribute("direction")) {
            glm::vec3 direction;
            sscanf(attr.as_string(), "%f,%f,%f", &direction.x, &direction.y, &direction.z);
            setDirection(direction);
        }
        if (auto attr = node.attribute("innerConeAngle")) {
            setInnerConeAngle(attr.as_float());
        }
        if (auto attr = node.attribute("outerConeAngle")) {
            setOuterConeAngle(attr.as_float());
        }
        if (auto attr = node.attribute("radius")) {
            setRadius(attr.as_float());
        }
        if (auto attr = node.attribute("castShadows")) {
            setCastShadows(attr.as_bool());
        }
    }

    void SpotLightComponent::serialize(pugi::xml_node& node) const {
        char colorStr[64];
        snprintf(colorStr, sizeof(colorStr), "%f,%f,%f", mColor.x, mColor.y, mColor.z);
        node.append_attribute("color") = colorStr;
        node.append_attribute("intensity") = mIntensity;
        
        char dirStr[64];
        snprintf(dirStr, sizeof(dirStr), "%f,%f,%f", mDirection.x, mDirection.y, mDirection.z);
        node.append_attribute("direction") = dirStr;
        node.append_attribute("innerConeAngle") = mInnerConeAngle;
        node.append_attribute("outerConeAngle") = mOuterConeAngle;
        node.append_attribute("radius") = mRadius;
        node.append_attribute("castShadows") = mCastShadows;
    }

}
#pragma once
#include "Component.h"

#include <glm/glm.hpp>
#include <pugixml.hpp>
#include <string>

namespace StrikeEngine {

    // Point light component
    class LightComponent : public Component {
    public:
        LightComponent() = default;
        LightComponent(const glm::vec3& color, float intensity, float radius, float fallOff);

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "light";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // Point light specific properties
        void setColor(const glm::vec3& color) { mColor = color; }
        const glm::vec3& getColor() const { return mColor; }

        void setIntensity(float intensity) { mIntensity = intensity; }
        float getIntensity() const { return mIntensity; }

        void setRadius(float radius) { mRadius = radius; }
        float getRadius() const { return mRadius; }

        void setFallOff(float fallOff) { mFallOff = fallOff; }
        float getFallOff() const { return mFallOff; }
    private:
        glm::vec3 mColor = glm::vec3(255.0f); // Default white (255,255,255)
        float mIntensity = 1.0f;              // Default intensity
        float mRadius = 10.0f;                // Default radius
        float mFallOff = 1.0f;                // Default falloff
    };
}
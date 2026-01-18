#pragma once
#include "Component.h"

#include <glm/glm.hpp>
#include <pugixml.hpp>
#include <string>

namespace StrikeEngine {

    class LightComponent : public Component {
    public:
        LightComponent() = default;
        LightComponent(const glm::uvec3& color, float intensity, float radius);

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "light";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // uvec3 interface (0–255 integers)
        void setColor(const glm::uvec3& color) { mColor = color; }
        const glm::uvec3& getColor() const     { return mColor; }

        // Optional: float vec3 convenience interface
        void setColorF(const glm::vec3& color) { mColor = glm::uvec3(color); }
        glm::vec3 getColorF() const            { return glm::vec3(mColor);   }

        void setIntensity(float intensity) { mIntensity = intensity; }
        float getIntensity() const { return mIntensity; }

        void setRadius(float radius) { mRadius = radius; }
        float getRadius() const { return mRadius; }

    private:
        glm::uvec3 mColor    = glm::uvec3(255, 255, 255); // now integer 0–255
        float      mIntensity = 1.0f;
        float      mRadius    = 10.0f;
    };
}
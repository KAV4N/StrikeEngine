#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <pugixml.hpp>
#include <string>

namespace StrikeEngine {

    class LightComponent : public Component {
    public:
        LightComponent() = default;
        virtual ~LightComponent() = default;

        void setColor(const glm::vec3& color) { mColor = color; }
        const glm::vec3& getColor() const { return mColor; }
        
        void setIntensity(float intensity) { mIntensity = intensity; }
        float getIntensity() const { return mIntensity; }

        void setCastShadows(bool cast) { mCastShadows = cast; }
        bool getCastShadows() const { return mCastShadows; }

    protected:
        glm::vec3 mColor = glm::vec3(1.0f); // Default white
        float mIntensity = 1.0f;            // Default intensity
        bool mCastShadows = false;         // Shadows off by default
    };

    // Point light component
    class PointLightComponent : public LightComponent {
    public:
        PointLightComponent() = default;
        PointLightComponent(const glm::vec3& color, float intensity, float radius);

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "pointLight";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // Point light specific properties
        void setRadius(float radius) { mRadius = radius; }
        float getRadius() const { return mRadius; }

        void setAttenuation(const glm::vec3& attenuation) { mAttenuation = attenuation; }
        const glm::vec3& getAttenuation() const { return mAttenuation; }

    private:
        float mRadius = 10.0f;                  // Default radius
        glm::vec3 mAttenuation = glm::vec3(1.0f, 0.09f, 0.032f); // Constant, linear, quadratic
    };

    // Directional light component
    class DirectionalLightComponent : public LightComponent {
    public:
        DirectionalLightComponent() = default;
        DirectionalLightComponent(const glm::vec3& color, float intensity, const glm::vec3& direction);

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "directionalLight";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // Directional light specific properties
        void setDirection(const glm::vec3& direction) { mDirection = glm::normalize(direction); }
        const glm::vec3& getDirection() const { return mDirection; }

    private:
        glm::vec3 mDirection = glm::vec3(0.0f, -1.0f, 0.0f); // Default downward direction
    };

    // Spot light component
    class SpotLightComponent : public LightComponent {
    public:
        SpotLightComponent() = default;
        SpotLightComponent(const glm::vec3& color, float intensity, const glm::vec3& direction, 
                          float innerConeAngle, float outerConeAngle);

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "spotLight";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

        // Spot light specific properties
        void setDirection(const glm::vec3& direction) { mDirection = glm::normalize(direction); }
        const glm::vec3& getDirection() const { return mDirection; }

        void setInnerConeAngle(float angle) { mInnerConeAngle = glm::clamp(angle, 0.0f, mOuterConeAngle); }
        float getInnerConeAngle() const { return mInnerConeAngle; }

        void setOuterConeAngle(float angle) { 
            mOuterConeAngle = glm::clamp(angle, mInnerConeAngle, 90.0f); 
        }
        float getOuterConeAngle() const { return mOuterConeAngle; }

        void setRadius(float radius) { mRadius = radius; }
        float getRadius() const { return mRadius; }

    private:
        glm::vec3 mDirection = glm::vec3(0.0f, -1.0f, 0.0f); // Default downward direction
        float mInnerConeAngle = 12.5f;                      // Default inner cone
        float mOuterConeAngle = 17.5f;                      // Default outer cone
        float mRadius = 10.0f;                              // Default radius
    };

}
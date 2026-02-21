#pragma once
#include "Component.h"

#include <glm/glm.hpp>
#include <pugixml.hpp>
#include <string>

namespace Strike {

    /**
     * @brief Component for point light sources in the scene.
     * 
     * The LightComponent represents a point light with color, intensity, and radius
     * properties for illuminating the scene.
     * 
     * @note Colors are stored as RGB integers in the range 0-255
     */
    class LightComponent : public Component {
    public:
        /**
         * @brief Default constructor
         * 
         * Creates a white light with default intensity (1.0) and radius (10.0)
         */
        LightComponent() = default;
        
        /**
         * @brief Construct light with specific properties
         * 
         * @param color RGB color values (0-255)
         * @param intensity Light intensity multiplier
         * @param radius Light influence radius in world units
         */
        LightComponent(const glm::uvec3& color, float intensity, float radius);

        /**
         * @brief Get the static type name for this component
         * 
         * @return Constant reference to the static type name "light"
         */
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "light";
            return typeName;
        }

        /**
         * @brief Get the type name of this component instance
         * 
         * @return Constant reference to the type name "light"
         */
        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        /**
         * @brief Deserialize light data from XML node
         * 
         * @param node The XML node containing light configuration
         */
        void deserialize(const pugi::xml_node& node) override;

        /**
         * @brief Set the light color
         * 
         * @param color RGB color values (0-255 per channel)
         * @note Values are automatically clamped to the valid range
         */
        void setColor(const glm::uvec3& color) { mColor = glm::clamp(color, glm::uvec3(0.0f), glm::uvec3(255.0f)); }
        
        /**
         * @brief Get the light color
         * 
         * @return Constant reference to RGB color values (0-255)
         */
        const glm::uvec3& getColor() const     { return mColor; }

        /**
         * @brief Set the light intensity
         * 
         * @param intensity Intensity multiplier (>= 0.0)
         * @note Negative values are clamped to 0.0
         */
        void setIntensity(float intensity) { mIntensity = glm::max(intensity, 0.0f); }
        
        /**
         * @brief Get the light intensity
         * 
         * @return Light intensity multiplier
         */
        float getIntensity() const { return mIntensity; }

        /**
         * @brief Set the light radius
         * 
         * @param radius Light influence radius in world units
         */
        void setRadius(float radius) { mRadius = radius; }
        
        /**
         * @brief Get the light radius
         * 
         * @return Light influence radius in world units
         */
        float getRadius() const { return mRadius; }

    private:
        glm::uvec3 mColor    = glm::uvec3(255, 255, 255); ///< RGB color (0-255)
        float      mIntensity = 1.0f;                     ///< Light intensity multiplier
        float      mRadius    = 10.0f;                    ///< Light influence radius
    };
}

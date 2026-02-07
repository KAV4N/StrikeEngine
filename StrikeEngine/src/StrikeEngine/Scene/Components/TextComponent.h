#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <string>

namespace StrikeEngine {

    /**
     * @brief Component for rendering 2D text overlays.
     * 
     * The TextComponent manages text rendering with properties for content, color,
     * position, and pivot point. Positions and pivots use normalized coordinates (0-1).
     * 
     * @note Colors are stored as RGB integers in the range 0-255
     */
    class TextComponent : public Component {
    public:
        /**
         * @brief Default constructor
         */
        TextComponent() = default;
        
        /**
         * @brief Virtual destructor
         */
        ~TextComponent() override = default;

        /**
         * @brief Get the static type name for this component
         * 
         * @return Constant reference to the static type name "text"
         */
        static const std::string& getStaticTypeName() {
            static const std::string typeName = "text";
            return typeName;
        }

        /**
         * @brief Get the type name of this component instance
         * 
         * @return Constant reference to the type name "text"
         */
        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        /**
         * @brief Deserialize text component data from XML node
         * 
         * @param node The XML node containing text configuration
         */
        void deserialize(const pugi::xml_node& node) override;

        /**
         * @brief Get the text content
         * 
         * @return Constant reference to the text string
         */
        const std::string& getText() const { return mText; }
        
        /**
         * @brief Get the text color
         * 
         * @return Constant reference to RGB color values (0-255)
         */
        const glm::uvec3& getColor() const { return mColor; }
        
        /**
         * @brief Get the pivot point
         * 
         * @return Constant reference to normalized pivot coordinates (0-1)
         * @note (0,0) = top-left, (0.5,0.5) = center, (1,1) = bottom-right
         */
        const glm::vec2& getPivot() const { return mPivot; }
        
        /**
         * @brief Get the screen position
         * 
         * @return Constant reference to normalized position coordinates (0-1)
         * @note (0,0) = top-left, (0.5,0.5) = center, (1,1) = bottom-right
         */
        const glm::vec2& getPosition() const { return mPosition; }

        /**
         * @brief Set the text content
         * 
         * @param text The text string to display
         */
        void setText(const std::string& text) { mText = text; }
        
        /**
         * @brief Set the text color
         * 
         * @param color RGB color values (0-255 per channel)
         * @note Values are automatically clamped to the valid range
         */
        void setColor(const glm::uvec3& color) { mColor = glm::clamp(color, glm::uvec3(0.0f), glm::uvec3(255.0f)); }
        
        /**
         * @brief Set the pivot point for text alignment
         * 
         * @param pivot Normalized pivot coordinates (0-1)
         * @note (0,0) = top-left corner as pivot, (0.5,0.5) = center, (1,1) = bottom-right
         */
        void setPivot(const glm::vec2& pivot) { mPivot = glm::clamp(pivot, glm::vec2(0.0f), glm::vec2(1.0f)); }
        
        /**
         * @brief Set the screen position for text rendering
         * 
         * @param position Normalized screen coordinates (0-1)
         * @note (0,0) = top-left of screen, (0.5,0.5) = screen center, (1,1) = bottom-right
         */
        void setPosition(const glm::vec2& position) { mPosition = glm::clamp(position, glm::vec2(0.0f), glm::vec2(1.0f)); }

    private:
        std::string mText = "";                           ///< Text content to display
        glm::uvec3 mColor = glm::uvec3(255.0f, 255.0f, 255.0f); ///< RGB color (0-255)
        glm::vec2 mPivot = glm::vec2(0.0f, 0.0f);        ///< Pivot point (0-1 normalized)
        glm::vec2 mPosition = glm::vec2(0.0f, 0.0f);     ///< Screen position (0-1 normalized)
    };

} 

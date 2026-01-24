#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <string>

namespace StrikeEngine {

    class TextComponent : public Component {
    public:
        TextComponent() = default;
        ~TextComponent() override = default;

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "text";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void deserialize(const pugi::xml_node& node) override;

        // Getters
        const std::string& getText() const { return mText; }
        const glm::uvec3& getColor() const { return mColor; }
        const glm::vec2& getPivot() const { return mPivot; }
        const glm::vec2& getPosition() const { return mPosition; }

        // Setters
        void setText(const std::string& text) { mText = text; }
        void setColor(const glm::uvec3& color) { mColor = glm::clamp(color, glm::uvec3(0.0f), glm::uvec3(255.0f)); }
        void setPivot(const glm::vec2& pivot) { mPivot = glm::clamp(pivot, glm::vec2(0.0f), glm::vec2(1.0f)); }
        void setPosition(const glm::vec2& position) { mPosition = glm::clamp(position, glm::vec2(0.0f), glm::vec2(1.0f)); }

    private:
        std::string mText = "";
        glm::uvec3 mColor = glm::uvec3(255.0f, 255.0f, 255.0f);
        glm::vec2 mPivot = glm::vec2(0.0f, 0.0f);
        glm::vec2 mPosition = glm::vec2(0.0f, 0.0f); // Normalized coordinates (0-1)
    };

} // namespace StrikeEngine
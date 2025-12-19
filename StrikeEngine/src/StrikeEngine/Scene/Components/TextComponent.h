#pragma once
#include "Component.h"
#include <glm/glm.hpp>

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
        void serialize(pugi::xml_node& node) const override;

        // Getters
        const std::string& getText() const { return mText; }
        const glm::vec3& getColor() const { return mColor; }

        // Setters
        void setText(const std::string& text) { mText = text; }
        void setColor(const glm::vec3& color) { mColor = color; }

    private:
        std::string mText = "";
        glm::vec3 mColor = glm::vec3(255.0f, 255.0f, 255.0f); 
    };
}
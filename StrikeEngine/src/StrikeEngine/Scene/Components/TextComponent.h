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
        void serialize(pugi::xml_node& node) const override;

        // Getters
        const std::string& getText() const { return mText; }
        const glm::vec3& getColor() const { return mColor; }
        const glm::vec2& getPivot() const { return mPivot; } 

        // Setters
        void setText(const std::string& text) { mText = text; }
        void setColor(const glm::vec3& color) { mColor = color; }
        void setPivot(const glm::vec2& pivot) { mPivot = glm::clamp(pivot, glm::vec2(0.0f), glm::vec2(1.0f)); }  
        void setPivot(float x, float y) { setPivot(glm::vec2(x, y)); }

    private:
        std::string mText = "";
        glm::vec3 mColor = glm::vec3(255.0f, 255.0f, 255.0f);
        glm::vec2 mPivot = glm::vec2(0.0f, 0.0f); 
    };

} // namespace StrikeEngine
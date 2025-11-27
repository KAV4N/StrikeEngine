#pragma once
#include "Component.h"

#include <string>
#include <pugixml.hpp>


namespace StrikeEngine {

    class TagComponent : public Component {
    public:
        TagComponent() = default;
        TagComponent(const std::string& tag) : mTag(tag) {}

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "tag";
            return typeName;
        }

        const std::string& getTypeName() const override { return getStaticTypeName(); }

        const std::string& getTag() const { return mTag; }
        void setTag(const std::string& tag) { mTag = tag; }

        void deserialize(const pugi::xml_node& node) override;
        void serialize(pugi::xml_node& node) const override;

    private:
        std::string mTag;
    };

} // namespace StrikeEngine
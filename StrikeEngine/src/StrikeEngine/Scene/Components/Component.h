#pragma once
#include <string>
#include <unordered_map>
#include <pugixml.hpp>
#include <memory>

namespace StrikeEngine {

    class Component {
    public:
        Component() = default;
        virtual ~Component() = default;
        virtual const std::string& getTypeName() const = 0;

        bool isActive() const { return mIsActive; }
        void setActive(bool active) { mIsActive = active; }

        // Virtual serialization methods
        virtual void deserialize(const pugi::xml_node& node) = 0;
        virtual void serialize(pugi::xml_node& node) const = 0;

    protected:
        bool mIsActive = true;
    };

}
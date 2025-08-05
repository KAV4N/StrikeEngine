#pragma once

#include <string>
#include <unordered_map>
#include <pugixml.hpp>

namespace StrikeEngine {

    class Component {
    public:
        Component() = default;
        virtual ~Component() = default;

        virtual const std::string& getTypeName() const = 0;
        virtual void deserializeAttributes(const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) = 0;
        virtual void serializeAttributes(pugi::xml_node& node) const = 0;
    };

    class ComponentSerializer {
    public:
        ComponentSerializer() = default;
        virtual ~ComponentSerializer() = default;

        virtual void deserialize(const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) = 0;
        virtual void serialize(pugi::xml_node& node) const = 0;

        bool isActive() { return mIsActive; }
        bool setActive(bool active) { mIsActive = active; }

    protected:
        bool mIsActive = true;
    };
}
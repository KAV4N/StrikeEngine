#pragma once
#include <string>
#include <unordered_map>
#include <pugixml.hpp>

namespace StrikeEngine {
    class Component {
    public:
        Component() = default;
        virtual ~Component() = default;
        virtual const std::string& GetTypeName() const = 0;
        virtual void SetAttributes(const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) = 0;
    };
}
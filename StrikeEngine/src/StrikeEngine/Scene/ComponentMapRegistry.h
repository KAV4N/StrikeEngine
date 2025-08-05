
#pragma once

#include "Components/Component.h"
#include <entt/entt.hpp>
#include <string>
#include <functional>
#include <unordered_map>
#include <pugixml.hpp>

namespace StrikeEngine {

    class Entity;

    class ComponentMapRegistry {
    public:
        using ComponentAdder = std::function<void(Entity&, const std::unordered_map<std::string, std::string>&, const pugi::xml_node&)>;

        static ComponentMapRegistry& get() {
            static ComponentMapRegistry instance;
            return instance;
        }

        template<typename T>
        void registerComponent() {
            /*
            static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
            T temp;
            const std::string& typeName = temp.getTypeName();
            mAdders[typeName] = [](Entity& entity, const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) {
                auto& component = entity.addComponent<T>();
                component.setAttributes(attributes, node);
                };
                */
        }

        bool hasComponent(const std::string& typeName) const;
        void addComponentToEntity(Entity& entity, const std::string& typeName, const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) const;

    private:
        ComponentMapRegistry();
        std::unordered_map<std::string, ComponentAdder> mAdders;
    };
}


#pragma once

#include "Components/Component.h"
#include <entt/entt.hpp>
#include <string>
#include <functional>
#include <unordered_map>
#include <pugixml.hpp>

namespace StrikeEngine {

    class Entity;

    class ComponentRegistry {
    public:
        using ComponentAdder = std::function<void(Entity&, const std::unordered_map<std::string, std::string>&, const pugi::xml_node&)>;

        static ComponentRegistry& get() {
            static ComponentRegistry instance;
            return instance;
        }

        template<typename T>
        void registerComponent() {
            static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
            const std::string& typeName = T::getStaticTypeName();
            mAdders[typeName] = [](Entity& entity, const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) {
                auto& component = entity.addComponent<T>();
                component.deserialize(attributes, node);
            };
        }

        bool isRegistered(const std::string& typeName) const;
        void addComponentToEntity(Entity& entity, const std::string& typeName, const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) const;

    private:
        ComponentRegistry();
        std::unordered_map<std::string, ComponentAdder> mAdders;
    };
}
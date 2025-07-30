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

        static ComponentRegistry& Get() {
            static ComponentRegistry instance;
            return instance;
        }

        template<typename T>
        void RegisterComponent() {
            static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
            T temp;
            const std::string& typeName = temp.GetTypeName();
            m_adders[typeName] = [](Entity& entity, const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) {
                auto& component = entity.AddComponent<T>();
                component.SetAttributes(attributes, node);
            };
        }

        bool HasComponent(const std::string& typeName) const;
        void AddComponentToEntity(Entity& entity, const std::string& typeName, const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) const;

    private:
        ComponentRegistry();
        std::unordered_map<std::string, ComponentAdder> m_adders;
    };
}
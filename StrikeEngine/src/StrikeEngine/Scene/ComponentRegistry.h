#pragma once

#include "Components/Component.h"
#include "Entity.h"

#include <entt/entt.hpp>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>
#include <pugixml.hpp>

namespace StrikeEngine {

    class ComponentRegistry {
    public:
        using ComponentFactory = std::function<Component*(Entity&, const pugi::xml_node&)>;

        static void registerComponentFactory(const std::string& typeName, ComponentFactory factory);

        static bool hasComponentFactory(const std::string& typeName);

        static void addComponentToEntity(Entity& entity, const std::string& typeName, const pugi::xml_node& node);

        static std::vector<std::string> getRegisteredComponents();

    private:
        static std::unordered_map<std::string, ComponentFactory>& getFactories();
    };

} // namespace StrikeEngine

#define REGISTER_COMPONENT(ComponentClass) \
    static bool ComponentClass##_registered = []() { \
        StrikeEngine::ComponentRegistry::registerComponentFactory(ComponentClass::getStaticTypeName(), \
            [](StrikeEngine::Entity& entity, const pugi::xml_node& node) -> StrikeEngine::Component* { \
                auto& component = entity.addComponent<ComponentClass>(); \
                component.deserialize(node); \
                return &component; \
            }); \
        return true; \
    }();
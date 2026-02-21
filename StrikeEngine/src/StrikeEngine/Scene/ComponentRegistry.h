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

namespace Strike {

    /**
     * @brief Registry for component factories to enable dynamic component creation.
     * @note Used for deserializing components from scene files.
     */
    class ComponentRegistry {
    public:
        /**
         * @brief Type definition for a component factory function.
         * @param entity The entity to which the component will be added.
         * @param node The XML node containing component data.
         * @return Pointer to the created Component.
         */
        using ComponentFactory = std::function<Component*(Entity&, const pugi::xml_node&)>;

        /**
         * @brief Register a component factory function with a type name.
         * @param typeName The name of the component type.
         * @param factory The factory function to create the component.
         */
        static void registerComponentFactory(const std::string& typeName, ComponentFactory factory);

        /**
         * @brief Check if a component factory is registered for the given type name.
         * @param typeName The name of the component type.
         * @return True if a factory is registered, false otherwise.
         */
        static bool hasComponentFactory(const std::string& typeName);

        /**
         * @brief Add a component of the specified type to the given entity using the provided XML node for initialization.
         * @param entity The entity to which the component will be added.
         * @param typeName The name of the component type.
         * @param node The XML node containing component data.
         * @note If the type name is not registered, no component is added.
         */
        static void addComponentToEntity(Entity& entity, const std::string& typeName, const pugi::xml_node& node);


           /**
         * @brief Get a list of all registered component class names.
         * @return Vector of registered component class names.
         */
        static std::vector<std::string> getRegisteredComponents();

    private:
        static std::unordered_map<std::string, ComponentFactory>& getFactories();
    };

} 


/**
 * @brief Macro to register a components with the ComponentRegistry.
 * @param ComponentClass The component to register.
 */
#define REGISTER_COMPONENT(ComponentClass) \
    static bool ComponentClass##_registered = []() { \
        Strike::ComponentRegistry::registerComponentFactory(ComponentClass::getStaticTypeName(), \
            [](Strike::Entity& entity, const pugi::xml_node& node) -> Strike::Component* { \
                auto& component = entity.addComponent<ComponentClass>(); \
                component.deserialize(node); \
                return &component; \
            }); \
        return true; \
    }();
#include "ComponentRegistry.h"

namespace StrikeEngine {

    std::unordered_map<std::string, ComponentRegistry::ComponentFactory>& ComponentRegistry::getFactories() {
        static std::unordered_map<std::string, ComponentFactory> sFactories;
        return sFactories;
    }

    void ComponentRegistry::registerComponentFactory(const std::string& typeName, ComponentFactory factory) {
        getFactories()[typeName] = factory;
    }

    bool ComponentRegistry::hasComponentFactory(const std::string& typeName) {
        return getFactories().find(typeName) != getFactories().end();
    }

    void ComponentRegistry::addComponentToEntity(Entity& entity, const std::string& typeName, const pugi::xml_node& node) {
        auto it = getFactories().find(typeName);
        if (it != getFactories().end()) {
            it->second(entity, node);
        }
    }

    std::vector<std::string> ComponentRegistry::getRegisteredComponents() {
        std::vector<std::string> components;
        for (const auto& pair : getFactories()) {
            components.push_back(pair.first);
        }
        return components;
    }

} // namespace StrikeEngine
#include "ComponentMapRegistry.h"
#include "Components/RendererComponent.h"
#include "Entity.h"

namespace StrikeEngine {

    ComponentMapRegistry::ComponentMapRegistry() {
        registerComponent<RendererComponent>();
    }

    bool ComponentMapRegistry::hasComponent(const std::string& typeName) const {
        return mAdders.find(typeName) != mAdders.end();
    }

    void ComponentMapRegistry::addComponentToEntity(Entity& entity, const std::string& typeName, const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) const {
        auto it = mAdders.find(typeName);
        if (it != mAdders.end()) {
            it->second(entity, attributes, node);
        }
    }
}

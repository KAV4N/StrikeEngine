#include "ComponentRegistry.h"
#include "Components/RendererComponent.h"
#include "Entity.h"

namespace StrikeEngine {

    ComponentRegistry::ComponentRegistry() {
        registerComponent<RendererComponent>();
    }

    bool ComponentRegistry::isRegistered(const std::string& typeName) const {
        return mAdders.find(typeName) != mAdders.end();
    }

    void ComponentRegistry::addComponentToEntity(Entity& entity, const std::string& typeName, const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) const {
        auto it = mAdders.find(typeName);
        if (it != mAdders.end()) {
            it->second(entity, attributes, node);
        }
    }
}
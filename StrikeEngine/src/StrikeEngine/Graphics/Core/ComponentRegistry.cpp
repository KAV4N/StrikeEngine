#include "ComponentRegistry.h"
#include "Components/RendererComponent.h"
#include "Entity.h"

namespace StrikeEngine {

    ComponentRegistry::ComponentRegistry() {
        RegisterComponent<RendererComponent>();
    }

    bool ComponentRegistry::HasComponent(const std::string& typeName) const {
        return m_adders.find(typeName) != m_adders.end();
    }

    void ComponentRegistry::AddComponentToEntity(Entity& entity, const std::string& typeName, const std::unordered_map<std::string, std::string>& attributes, const pugi::xml_node& node) const {
        auto it = m_adders.find(typeName);
        if (it != m_adders.end()) {
            it->second(entity, attributes, node);
        }
    }

}
#include "ComponentRegistry.h"
#include "Components/RendererComponent.h"
#include "Components/CameraComponent.h"
#include "Components/ScriptComponent.h"
#include "Entity.h"

namespace StrikeEngine {

    ComponentRegistry::ComponentRegistry() {
        registerComponent<RendererComponent>();
        registerComponent<CameraComponent>();
        registerComponent<ScriptComponent>();
    }

    bool ComponentRegistry::isRegistered(const std::string& typeName) const {
        return mAdders.find(typeName) != mAdders.end();
    }

    void ComponentRegistry::addComponentToEntity(Entity& entity, const std::string& typeName, const pugi::xml_node& node) const {
        auto it = mAdders.find(typeName);
        if (it != mAdders.end()) {
            it->second(entity, node);
        }
    }
}
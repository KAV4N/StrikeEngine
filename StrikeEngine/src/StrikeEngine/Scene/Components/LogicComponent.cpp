#include "strikepch.h"
#include "LogicComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

namespace StrikeEngine {
    REGISTER_COMPONENT(LogicComponent)
      
    void LogicComponent::clearScripts() {
        mScripts.clear();
    }

    Script* LogicComponent::addScript(const std::string& scriptTypeName) {
        if (!ScriptRegistry::hasScriptFactory(scriptTypeName)) {
            return nullptr;
        }

        auto script = ScriptRegistry::createScript(scriptTypeName);
        if (!script) {
            return nullptr;
        }

        Script* scriptPtr = script.get();
        mScripts.push_back(std::move(script));
        
        return scriptPtr;
    }

    void LogicComponent::deserialize(const pugi::xml_node& node) {
        clearScripts();

        for (auto scriptNode : node.children("script")) {
            if (auto attr = scriptNode.attribute("type")) {
                std::string scriptType = attr.as_string();
                addScript(scriptType);
            }
        }

        if (auto attr = node.attribute("active")) {
            setActive(attr.as_bool(true));
        }

    }
} 
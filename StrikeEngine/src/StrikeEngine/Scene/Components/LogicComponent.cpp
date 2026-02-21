#include "strikepch.h"
#include "LogicComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

namespace StrikeEngine {
    REGISTER_COMPONENT(LogicComponent)
      
    void LogicComponent::clearScripts() {
        mScripts.clear();
        mScriptTypeNames.clear();
    }

    Script* LogicComponent::addScript(const std::string& scriptTypeName) {
        if (!ScriptRegistry::hasScriptFactory(scriptTypeName)) {
            STRIKE_CORE_ERROR("LogicComponent::addScript: No factory registered for script type '{}'!", scriptTypeName);
            return nullptr;
        }

        // Check if a script of this type is already attached
        for (size_t i = 0; i < mScriptTypeNames.size(); ++i) {
            if (mScriptTypeNames[i] == scriptTypeName) {
                STRIKE_CORE_ERROR("LogicComponent::addScript: Script of type '{}' is already attached to this component!", scriptTypeName);
                return mScripts[i].get();
            }
        }

        auto script = ScriptRegistry::createScript(scriptTypeName);
        if (!script) {
            return nullptr;
        }

        Script* scriptPtr = script.get();
        mScriptTypeNames.push_back(scriptTypeName);
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
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
    }

    void LogicComponent::serialize(pugi::xml_node& node) const {
        for (const auto& script : mScripts) {
            auto scriptNode = node.append_child("script");
            scriptNode.append_attribute("type") = typeid(*script).name();
        }
    }

} 
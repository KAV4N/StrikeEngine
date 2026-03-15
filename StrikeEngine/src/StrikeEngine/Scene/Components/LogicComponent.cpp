#include "strikepch.h"
#include "LogicComponent.h"
#include "StrikeEngine/Scene/ComponentRegistry.h"

namespace Strike {
    REGISTER_COMPONENT(LogicComponent)

    LogicComponent::~LogicComponent() {
        for (auto& script : mScripts) {
            if (script) {
                script->onDestroy();
            }
        }
        mScripts.clear();
        mScriptTypeNames.clear();
    }

    void LogicComponent::clearScripts() {
        for (auto& script : mScripts) {
            if (script) {
                script->onDestroy();
            }
        }
        mScripts.clear();
        mScriptTypeNames.clear();
    }

    Script& LogicComponent::addScript(const std::string& scriptTypeName) {
        STRIKE_CORE_ASSERT(ScriptRegistry::hasScriptFactory(scriptTypeName),
            "LogicComponent::addScript: No factory registered for script type '{}'!", scriptTypeName);

        for (size_t i = 0; i < mScriptTypeNames.size(); ++i) {
            if (mScriptTypeNames[i] == scriptTypeName) {
                STRIKE_CORE_WARN("LogicComponent::addScript: Script of type '{}' is already attached to this component!", scriptTypeName);
                return *mScripts[i];
            }
        }

        auto script = ScriptRegistry::createScript(scriptTypeName);
        Script& ref = *script;
        mScriptTypeNames.push_back(scriptTypeName);
        mScripts.push_back(std::move(script));

        return ref;
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
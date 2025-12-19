#include "strikepch.h"
#include "ScriptRegistry.h"

namespace StrikeEngine {

    std::unordered_map<std::string, ScriptRegistry::ScriptFactory>& ScriptRegistry::getFactories() {
        static std::unordered_map<std::string, ScriptFactory> sFactories;
        return sFactories;
    }

    void ScriptRegistry::registerScriptFactory(const std::string& className, ScriptFactory factory) {
        getFactories()[className] = factory;
    }

    bool ScriptRegistry::hasScriptFactory(const std::string& className) {
        return getFactories().find(className) != getFactories().end();
    }

    std::unique_ptr<Script> ScriptRegistry::createScript(const std::string& className) {
        auto it = getFactories().find(className);
        if (it != getFactories().end()) {
            return it->second();
        }
        return nullptr;
    }

    std::vector<std::string> ScriptRegistry::getRegisteredScripts() {
        std::vector<std::string> scripts;
        for (const auto& pair : getFactories()) {
            scripts.push_back(pair.first);
        }
        return scripts;
    }



} // namespace StrikeEngine
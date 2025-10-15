#pragma once
#include "StrikeEngine/Scene/Script.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

namespace StrikeEngine {

    class ScriptRegistry {
    public:
        using ScriptFactory = std::function<std::unique_ptr<Script>()>;

        // Register a script factory with the class name
        static void registerScriptFactory(const std::string& className, ScriptFactory factory);

        // Check if a script factory exists
        static bool hasScriptFactory(const std::string& className);

        // Create a script instance by class name
        static std::unique_ptr<Script> createScript(const std::string& className);

        // Get all registered script class names
        static std::vector<std::string> getRegisteredScripts();

    private:
        static std::unordered_map<std::string, ScriptFactory>& getFactories();
    };


} // namespace StrikeEngine

    // Macro to register scripts - now only takes the class
#define REGISTER_SCRIPT(ScriptClass) \
    static bool ScriptClass##_registered = []() { \
        StrikeEngine::ScriptRegistry::registerScriptFactory(#ScriptClass, \
            []() -> std::unique_ptr<StrikeEngine::Script> { \
                return std::make_unique<ScriptClass>(); \
            }); \
        return true; \
    }();

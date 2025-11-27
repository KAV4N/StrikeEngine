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

        static void registerScriptFactory(const std::string& className, ScriptFactory factory);

        static bool hasScriptFactory(const std::string& className);

        static std::unique_ptr<Script> createScript(const std::string& className);

        static std::vector<std::string> getRegisteredScripts();

    private:
        static std::unordered_map<std::string, ScriptFactory>& getFactories();
    };


} 


#define REGISTER_SCRIPT(ScriptClass) \
    static bool ScriptClass##_registered = []() { \
        StrikeEngine::ScriptRegistry::registerScriptFactory(#ScriptClass, \
            []() -> std::unique_ptr<StrikeEngine::Script> { \
                return std::make_unique<ScriptClass>(); \
            }); \
        return true; \
    }();

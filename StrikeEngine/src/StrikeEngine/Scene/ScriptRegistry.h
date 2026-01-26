#pragma once
#include "StrikeEngine/Scene/Script.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

namespace StrikeEngine {


    /**
     * @brief Registry for script classes to enable dynamic instantiation by class name.
     */
    class ScriptRegistry {
    public:
        /**
         * @brief Type definition for a script factory function.
         * @return Unique pointer to the created Script instance.
         * 
         */
        using ScriptFactory = std::function<std::unique_ptr<Script>()>;

        /**
         * @brief Register a script factory function with a class name.
         * @param className The name of the script class.
         */
        static void registerScriptFactory(const std::string& className, ScriptFactory factory);

        /**
         * @brief Check if a script factory is registered for the given class name.
         * @param className The name of the script class.
         * @return True if a factory is registered, false otherwise.
         */
        static bool hasScriptFactory(const std::string& className);

        /**
         * @brief Create an instance of a script by class name.
         * @param className The name of the script class.
         * @return Unique pointer to the created script instance, or nullptr if not found.
         */
        static std::unique_ptr<Script> createScript(const std::string& className);

        /**
         * @brief Get a list of all registered script class names.
         * @return Vector of registered script class names.
         */
        static std::vector<std::string> getRegisteredScripts();

    private:
        static std::unordered_map<std::string, ScriptFactory>& getFactories();
    };


} 

/**
 * @brief Macro to register a script class with the ScriptRegistry.
 * @param ScriptClass The script class to register.
 */
#define REGISTER_SCRIPT(ScriptClass) \
    static bool ScriptClass##_registered = []() { \
        StrikeEngine::ScriptRegistry::registerScriptFactory(#ScriptClass, \
            []() -> std::unique_ptr<StrikeEngine::Script> { \
                return std::make_unique<ScriptClass>(); \
            }); \
        return true; \
    }();

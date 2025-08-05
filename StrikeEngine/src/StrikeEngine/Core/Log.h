#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace StrikeEngine {

    class Log {
    public:
        static void init();

        inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return sCoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return sClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> sCoreLogger;
        static std::shared_ptr<spdlog::logger> sClientLogger;
    };
}

// Core log macros
#define STRIKE_CORE_ERROR(...)      ::StrikeEngine::Log::getCoreLogger()->error(__VA_ARGS__)
#define STRIKE_CORE_WARN(...)       ::StrikeEngine::Log::getCoreLogger()->warn(__VA_ARGS__)
#define STRIKE_CORE_INFO(...)       ::StrikeEngine::Log::getCoreLogger()->info(__VA_ARGS__)
#define STRIKE_CORE_TRACE(...)      ::StrikeEngine::Log::getCoreLogger()->trace(__VA_ARGS__)

// Client log macros
#define STRIKE_ERROR(...)           ::StrikeEngine::Log::getClientLogger()->error(__VA_ARGS__)
#define STRIKE_WARN(...)            ::StrikeEngine::Log::getClientLogger()->warn(__VA_ARGS__)
#define STRIKE_INFO(...)            ::StrikeEngine::Log::getClientLogger()->info(__VA_ARGS__)
#define STRIKE_TRACE(...)           ::StrikeEngine::Log::getClientLogger()->trace(__VA_ARGS__)
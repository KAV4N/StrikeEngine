#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Strike {

    /**
     * @class Log
     * @brief Centralized logging system for StrikeEngine.
     *
     * The Log class wraps the spdlog library and provides
     * separate loggers for the engine core and the client.
     * It must be initialized once at application startup
     * before any logging macros are used.
     */
    class Log {
    public:
        /**
         * @brief Initializes the logging system.
         *
         * Creates and configures both the core and client loggers,
         * including log levels, formatting, and output sinks.
         * This function should be called once during engine startup.
         */
        static void init();

        /**
         * @brief Retrieves the engine core logger.
         *
         * Intended for internal engine logging.
         *
         * @return Reference to the shared pointer of the core logger.
         */
        inline static std::shared_ptr<spdlog::logger>& getCoreLogger() {
            return sCoreLogger;
        }

        /**
         * @brief Retrieves the client logger.
         *
         * Intended for application or game-level logging.
         *
         * @return Reference to the shared pointer of the client logger.
         */
        inline static std::shared_ptr<spdlog::logger>& getClientLogger() {
            return sClientLogger;
        }

    private:
        /**
         * @brief Logger used internally by the engine core.
         */
        static std::shared_ptr<spdlog::logger> sCoreLogger;

        /**
         * @brief Logger exposed to the client application.
         */
        static std::shared_ptr<spdlog::logger> sClientLogger;
    };
}

/**
 * @defgroup CoreLoggingMacros Core Logging Macros
 * @brief Convenience macros for engine core logging.
 * @{
 */

/**
 * @brief Logs an error message to the core logger.
 */
#define STRIKE_CORE_ERROR(...)  ::Strike::Log::getCoreLogger()->error(__VA_ARGS__)

/**
 * @brief Logs a warning message to the core logger.
 */
#define STRIKE_CORE_WARN(...)   ::Strike::Log::getCoreLogger()->warn(__VA_ARGS__)

/**
 * @brief Logs an informational message to the core logger.
 */
#define STRIKE_CORE_INFO(...)   ::Strike::Log::getCoreLogger()->info(__VA_ARGS__)

/**
 * @brief Logs a trace-level message to the core logger.
 */
#define STRIKE_CORE_TRACE(...)  ::Strike::Log::getCoreLogger()->trace(__VA_ARGS__)

/** @} */

/**
 * @defgroup ClientLoggingMacros Client Logging Macros
 * @brief Convenience macros for client/application logging.
 * @{
 */

/**
 * @brief Logs an error message to the client logger.
 */
#define STRIKE_ERROR(...)       ::Strike::Log::getClientLogger()->error(__VA_ARGS__)

/**
 * @brief Logs a warning message to the client logger.
 */
#define STRIKE_WARN(...)        ::Strike::Log::getClientLogger()->warn(__VA_ARGS__)

/**
 * @brief Logs an informational message to the client logger.
 */
#define STRIKE_INFO(...)        ::Strike::Log::getClientLogger()->info(__VA_ARGS__)

/**
 * @brief Logs a trace-level message to the client logger.
 */
#define STRIKE_TRACE(...)       ::Strike::Log::getClientLogger()->trace(__VA_ARGS__)

/** @} */

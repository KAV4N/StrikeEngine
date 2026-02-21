#include "strikepch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Strike {

    std::shared_ptr<spdlog::logger> Log::sCoreLogger;
    std::shared_ptr<spdlog::logger> Log::sClientLogger;

    void Log::init() {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        sCoreLogger = spdlog::stdout_color_mt("STRIKE");
        sCoreLogger->set_level(spdlog::level::trace);

        sClientLogger = spdlog::stdout_color_mt("APP");
        sClientLogger->set_level(spdlog::level::trace);
    }
}
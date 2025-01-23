#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"


namespace StrikeEngine 
{
	class Log 
	{

	public:
		static void Init();
		
		inline static std::shared_ptr <spdlog::logger>& GetCoreLogger() { return s_CoreLogger;  }
		inline static std::shared_ptr <spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

//Core log macros
//#define STRIKE_CORE_FATAL(...)      ::StrikeEngine::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#define STRIKE_CORE_ERROR(...)      ::StrikeEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define STRIKE_CORE_WARN(...)       ::StrikeEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define STRIKE_CORE_INFO(...)       ::StrikeEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define STRIKE_CORE_TRACE(...)      ::StrikeEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)


//Client log macros
//#define STRIKE_FATAL(...)           ::StrikeEngine::Log::GetClientLogger()->fatal(__VA_ARGS__)
#define STRIKE_ERROR(...)           ::StrikeEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define STRIKE_WARN(...)            ::StrikeEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define STRIKE_INFO(...)            ::StrikeEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define STRIKE_TRACE(...)           ::StrikeEngine::Log::GetClientLogger()->trace(__VA_ARGS__)



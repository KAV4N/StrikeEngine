#pragma once

#ifdef PLATFORM_WINDOWS
	#ifdef BUILD_DLL
		#define STRIKE_API __declspec(dllexport)
	#else
		#define STRIKE_API __declspec(dllimport)
	#endif
#else
	#error Only Windows OS is supported!
#endif

#ifdef STRIKE_ENABLE_ASSERTS
	#define STRIKE_ASSERT(x, ...) { if(!(x)) { STRIKE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define STRIKE_CORE_ASSERT(x, ...) { if(!(x)) { STRIKE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define STRIKE_ASSERT(x, ...)
	#define STRIKE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
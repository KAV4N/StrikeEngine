#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#ifdef STRIKE_DEBUG
    #define STRIKE_ENABLE_ASSERTS
#endif

// Platform-specific debug break
#if defined(_MSC_VER)
    #define STRIKE_DEBUGBREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
    #define STRIKE_DEBUGBREAK() __builtin_trap()
#else
    #include <csignal>
    #define STRIKE_DEBUGBREAK() raise(SIGTRAP)
#endif

#ifdef STRIKE_ENABLE_ASSERTS
    #define STRIKE_ASSERT(x, ...) { if(!(x)) { STRIKE_ERROR("Assertion Failed: {0}", ##__VA_ARGS__); STRIKE_DEBUGBREAK(); } }
    #define STRIKE_CORE_ASSERT(x, ...) { if(!(x)) { STRIKE_CORE_ERROR("Assertion Failed: {0}", ##__VA_ARGS__); STRIKE_DEBUGBREAK(); } }
#else
    #define STRIKE_ASSERT(x, ...)
    #define STRIKE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
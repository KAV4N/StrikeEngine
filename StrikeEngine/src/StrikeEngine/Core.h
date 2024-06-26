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
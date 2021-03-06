#pragma once

// Only use in Platform project

#include "PlatformDebugUtility.h"
#include "Base/Format.h"

namespace cube
{
	namespace platform
	{
		template <typename ...Args>
		void AssertionFailed(const char* fileName, int lineNum, StringView msg, Args&&... args)
		{
			platform::PlatformDebugUtility::AssertionFailed(Format(msg.data(), std::forward<Args>(args)...), nullptr, fileName, lineNum);
		}

		void PrintConsoleImpl(const char* fileName, int lineNum, String&& msg);
		template <typename ...Args>
		void PrintConsole(const char* fileName, int lineNum, StringView msg, Args&&... args)
		{
			PrintConsoleImpl(fileName, lineNum, Format(msg.data(), std::forward<Args>(args)...));
		}
	} // namespace core
} // namespace cube

#ifdef _DEBUG

#define PLATFORM_ASSERTION_FAILED(msg, ...)                                          \
	cube::platform::AssertionFailed(__FILE__, __LINE__, CUBE_T(msg), ##__VA_ARGS__);

#define PLATFORM_CHECK(expr, msg, ...)                 \
	if(!(expr)){                                       \
		PLATFORM_ASSERTION_FAILED(msg, ##__VA_ARGS__); \
	}

#define PLATFORM_PRINT_LOG(msg, ...)                                              \
	cube::platform::PrintConsole(__FILE__, __LINE__, CUBE_T(msg), ##__VA_ARGS__);

#else // _DEBUG

#define PLATFORM_ASSERTION_FAILED(msg, ...)
#define PLATFORM_CHECK(expr, msg, ...)
#define PLATFORM_PRINT_LOG(msg, ...)

#endif // _DEBUG

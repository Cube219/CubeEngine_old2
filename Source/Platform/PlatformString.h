#pragma once

#include "PlatformHeader.h"

namespace cube
{
	// TODO: PString 전용 Memory pool allocator 구현

#ifdef _WIN32
	using PCharacter = wchar_t;
	using PString = std::wstring;
#endif

	PLATFORM_EXPORT PString ToPString(const U8Character* str);
	inline PString ToPString(const U8String& str)
	{
		return ToPString(str.data());
	}

	PLATFORM_EXPORT PString ToPString(const U16Character* str);
	inline PString ToPString(const U16String& str)
	{
		return ToPString(str.data());
	}

	PLATFORM_EXPORT PString ToPString(const U32Character* str);
	inline PString ToPString(const U32String& str)
	{
		return ToPString(str.data());
	}

	PLATFORM_EXPORT U8String ToU8String(const PString& str);
	PLATFORM_EXPORT U16String ToU16String(const PString& str);
	PLATFORM_EXPORT U32String ToU32String(const PString& str);
} // namespace cube

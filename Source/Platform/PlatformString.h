#pragma once

#include "PlatformHeader.h"

namespace cube
{
	// TODO: PString 전용 Memory pool allocator 구현

#ifdef _WIN32
	using PCharacter = wchar_t;
	using PString = std::wstring;
	using PStringView = BaseStringView<wchar_t>;
#endif

	PLATFORM_EXPORT PString ToPString(U8StringView str);
	PLATFORM_EXPORT PString ToPString(U16StringView str);
	PLATFORM_EXPORT PString ToPString(U32StringView str);

	PLATFORM_EXPORT U8String ToU8String(PStringView str);
	PLATFORM_EXPORT U16String ToU16String(PStringView str);
	PLATFORM_EXPORT U32String ToU32String(PStringView str);
} // namespace cube

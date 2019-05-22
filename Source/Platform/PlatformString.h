#pragma once

#include "PlatformHeader.h"

namespace cube
{
	// TODO: PString 전용 Memory pool allocator 구현

#ifdef _WIN32
	#define PLATFORM_T(text) L ## text
	using PCharacter = wchar_t;
	using PString = eastl::basic_string<PCharacter>;
	using PStringView = eastl::basic_string_view<wchar_t>;
	
	namespace internal
	{
		char32_t DecodeAndMoveInPCharacter(const wchar_t*& pStr);
		int EncodeInPCharacter(char32_t code, wchar_t* pStr);
	}
#endif // _WIN32
} // namespace cube

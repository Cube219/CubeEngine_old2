#ifdef _WIN32

#include "../PlatformString.h"

#include <Windows.h>
#include <iostream>
#include <mutex>

#include "../PlatformAssertion.h"

namespace cube
{
	PString ToPString(U8StringView str)
	{
		int pStrLength = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);
		PLATFORM_CHECK(pStrLength > 0, "Failed to convert UTF8 to WString (Error code: {0})", GetLastError());

		PString pStr;
		pStr.resize(pStrLength);

		int res = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &pStr[0], pStrLength);
		PLATFORM_CHECK(res != 0, "Failed to convert UTF8 to WString (Error code: {0})", GetLastError());

		return pStr;
	}
	PString ToPString(U16StringView str)
	{
		PString pStr;
		pStr.reserve(str.size());

		for(auto iter = str.begin(); iter != str.end(); iter++) {
			pStr.push_back((wchar_t)*iter);
		}

		return pStr;
	}
	PString ToPString(U32StringView str)
	{
		int pStrLength = 0;
		for(auto iter = str.begin(); iter != str.end(); iter++) {
			if((*iter & 0xFFFF0000) == 0) {
				pStrLength += 1;
			} else {
				pStrLength += 2;
			}
		}

		PString pStr;
		pStr.reserve(pStrLength);

		for(auto iter = str.begin(); iter != str.end(); iter++) {
			if((*iter & 0xFFFF0000) == 0) {
				pStr.push_back((wchar_t)*iter);
			} else {
				char32_t high = 0xD800 + ((*iter - 0x10000) >> 10);
				char32_t low = 0xDC00 + ((*iter - 0x10000) & 0b1111111111);

				pStr.push_back((wchar_t)high);
				pStr.push_back((wchar_t)low);
			}
		}

		return pStr;
	}

	U8String ToU8String(PStringView str)
	{
		int u8StrLength = WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0, NULL, NULL);
		PLATFORM_CHECK(u8StrLength > 0, "Failed to convert WString to UTF8 (Error code: {0})", GetLastError());

		U8String u8Str;
		u8Str.resize(u8StrLength);

		int res = WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.size(), &u8Str[0], u8StrLength, NULL, NULL);
		PLATFORM_CHECK(res != 0, "Failed to convert WString to UTF8 (Error code: {0})", GetLastError());

		return u8Str;
	}
	U16String ToU16String(PStringView str)
	{
		U16String u16Str;
		u16Str.reserve(str.size());

		for(auto iter = str.begin(); iter != str.end(); iter++) {
			u16Str.push_back(*iter);
		}

		return u16Str;
	}
	U32String ToU32String(PStringView str)
	{
		int u32StrLength = 0;
		for(auto iter = str.begin(); iter != str.end(); iter++) {
			if((*iter & 0xFC00) == 0xD800) {
				iter++;
			}
			u32StrLength++;
		}

		U32String u32Str;
		u32Str.reserve(u32StrLength);

		for(auto iter = str.begin(); iter != str.end(); iter++) {
			if((*iter & 0xFC00) == 0xD800) {
				wchar_t high = *iter;
				iter++;
				wchar_t low = *iter;

				char32_t res = ((high - 0xD800) << 10) + (low - 0xDC00) + 0x10000;
				u32Str.push_back(res);
			} else {
				u32Str.push_back(*iter);
			}
		}

		return u32Str;
	}
} // namespace cube

#endif // _WIN32

#ifdef _WIN32

#include "../PlatformString.h"

#include <Windows.h>
#include <iostream>
#include <mutex>

#include "../PlatformAssertion.h"

namespace cube
{
	PString ToPString(const U8Character* str)
	{
		int strLength = strnlen_s(str, 1000);

		int pStrLength = MultiByteToWideChar(CP_UTF8, 0, str, strLength, nullptr, 0);
		PLATFORM_CHECK(pStrLength > 0, "Failed to convert UTF8 to WString (Error code: {0})", GetLastError());

		PString pStr;
		pStr.resize(pStrLength);

		int res = MultiByteToWideChar(CP_UTF8, 0, str, strLength, &pStr[0], pStrLength);
		PLATFORM_CHECK(res != 0, "Failed to convert UTF8 to WString (Error code: {0})", GetLastError());

		return pStr;
	}
	PString ToPString(const U16Character* str)
	{
		int strLength = 0;
		while(str[strLength] != u'\0') {
			++strLength;
		}

		PString pStr;
		pStr.reserve(strLength);

		for(int i = 0; i < strLength; ++i) {
			pStr.push_back((wchar_t)str[i]);
		}

		return pStr;
	}
	PString ToPString(const U32Character* str)
	{
		int strLength = 0;
		while(str[strLength] != U'\0') {
			++strLength;
		}

		int pStrLength = 0;
		for(int i = 0; i < strLength; ++i) {
			if((str[i] & 0xFFFF0000) == 0) {
				pStrLength += 1;
			} else {
				pStrLength += 2;
			}
		}

		PString pStr;
		pStr.reserve(pStrLength);

		for(int i = 0; i < strLength; ++i) {
			if((str[i] & 0xFFFF0000) == 0) {
				pStr.push_back((wchar_t)str[i]);
			} else {
				char32_t high = 0xD800 + ((str[i] - 0x10000) >> 10);
				char32_t low = 0xDC00 + ((str[i] - 0x10000) & 0b1111111111);

				pStr.push_back((wchar_t)high);
				pStr.push_back((wchar_t)low);
			}
		}

		return pStr;
	}

	U8String ToU8String(const PString& str)
	{
		int u8StrLength = WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0, NULL, NULL);
		PLATFORM_CHECK(u8StrLength > 0, "Failed to convert WString to UTF8 (Error code: {0})", GetLastError());

		U8String u8Str;
		u8Str.resize(u8StrLength);

		int res = WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.size(), &u8Str[0], u8StrLength, NULL, NULL);
		PLATFORM_CHECK(res != 0, "Failed to convert WString to UTF8 (Error code: {0})", GetLastError());

		return u8Str;
	}
	U16String ToU16String(const PString& str)
	{
		U16String u16Str;
		u16Str.reserve(str.size());

		for(auto iter = str.cbegin(); iter != str.cend(); iter++) {
			u16Str.push_back(*iter);
		}

		return u16Str;
	}
	U32String ToU32String(const PString& str)
	{
		int u32StrLength = 0;
		for(auto iter = str.cbegin(); iter != str.cend(); iter++) {
			if((*iter & 0xFC00) == 0xD800) {
				iter++;
			}
			u32StrLength++;
		}

		U32String u32Str;
		u32Str.reserve(u32StrLength);

		for(auto iter = str.cbegin(); iter != str.cend(); iter++) {
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

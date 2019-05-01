#include "String.h"

#include <iostream>

namespace cube
{
	//////////////
	// U8String //
	//////////////
	U8String ToU8String(const U16String& str)
	{
		int u8StrLength = 0;
		for(auto iter = str.cbegin(); iter != str.cend();) {
			char32_t codePoint = internal::GetUTF16CharAndMove(iter);
			u8StrLength += internal::GetUTF8RequiredCharSize(codePoint);
		}

		U8String u8Str;
		u8Str.reserve(u8StrLength);
		for(auto iter = str.cbegin(); iter != str.cend();) {
			char32_t codePoint = internal::GetUTF16CharAndMove(iter);
			internal::InsertCharInUTF8(u8Str, codePoint);
		}

		return u8Str;
	}
	U8String ToU8String(const U32String& str)
	{
		int u8StrLength = 0;
		for(auto iter = str.cbegin(); iter != str.cend(); iter++) {
			char32_t ch = *iter;

			u8StrLength += internal::GetUTF8RequiredCharSize(*iter);
		}

		U8String u8Str;
		u8Str.reserve(u8StrLength);
		for(auto iter = str.cbegin(); iter != str.cend(); iter++) {
			internal::InsertCharInUTF8(u8Str, *iter);
		}

		return u8Str;
	}

	///////////////
	// U16String //
	///////////////
	U16String ToU16String(const U8String& str)
	{
		int u16StrLength = 0;
		for(auto iter = str.cbegin(); iter != str.cend();) {
			int chSize = internal::GetUTF8CharSize(iter);
			iter += chSize;

			if(chSize <= 3)
				u16StrLength += 1;
			else
				u16StrLength += 2;
		}

		U16String u16Str;
		u16Str.reserve(u16StrLength);
		for(auto iter = str.cbegin(); iter != str.cend();) {
			char32_t ch = internal::GetUTF8CharAndMove(iter);

			internal::InsertCharInUTF16(u16Str, ch);
		}

		return u16Str;
	}
	U16String ToU16String(const U32String& str)
	{
		int u16StrLength = 0;
		for(auto iter = str.cbegin(); iter != str.cend(); iter++) {
			u16StrLength += internal::GetUTF16RequiredCharSize(*iter);
		}

		U16String u16Str;
		for(auto iter = str.cbegin(); iter != str.cend(); iter++) {
			internal::InsertCharInUTF16(u16Str, *iter);
		}

		return u16Str;
	}

	///////////////
	// U32String //
	///////////////
	U32String ToU32String(const U8String& str)
	{
		int u32StrLength = 0;
		for(auto iter = str.cbegin(); iter != str.cend();) {
			u32StrLength++;

			int chSize = internal::GetUTF8CharSize(iter);
			iter += chSize;
		}

		U32String u32Str;
		for(auto iter = str.cbegin(); iter != str.cend();) {
			char32_t ch = internal::GetUTF8CharAndMove(iter);
			u32Str.push_back(ch);
		}

		return u32Str;
	}
	U32String ToU32String(const U16String& str)
	{
		int u32StrLength = 0;
		for(auto iter = str.cbegin(); iter != str.cend();) {
			u32StrLength++;

			int chSize = internal::GetUTF16CharSize(iter);
			iter += chSize;
		}

		U32String u32Str;
		for(auto iter = str.cbegin(); iter != str.cend();) {
			char32_t ch = internal::GetUTF16CharAndMove(iter);
			u32Str.push_back(ch);
		}

		return u32Str;
	}

	namespace internal
	{
		char32_t GetUTF8CharAndMove(U8String::const_iterator& iter)
		{
			char32_t res = 0;
			char first, second, third, fourth;

			int chSize = GetUTF8CharSize(iter);
			char ch = *iter;

			switch(chSize) {
				case 1:
					res = ch;
					++iter;
					break;

				case 2:
					first = ch & 0x1F;
					ch = *(++iter);
					second = ch & 0x3F;
					++iter;

					res = (first << 6) | second;
					break;

				case 3:
					first = ch & 0xF;
					ch = *(++iter);
					second = ch & 0x3F;
					ch = *(++iter);
					third = ch & 0x3F;
					++iter;

					res = (first << 12) | (second << 6) | third;
					break;

				case 4:
					first = ch & 0x7;
					ch = *(++iter);
					second = ch & 0x3F;
					ch = *(++iter);
					third = ch & 0x3F;
					ch = *(++iter);
					fourth = ch & 0x3F;
					++iter;

					res = (first << 18) | (second << 12) | (third << 6) | fourth;
					break;
			}

			return res;
		}
		int GetUTF8CharSize(U8String::const_iterator iter)
		{
			char ch = *iter;

			if((ch & 0x80) == 0) {
				return 1;
			} else if((ch & 0xE0) == 0xC0) {
				return 2;
			} else if((ch & 0xF0) == 0xE0) {
				return 3;
			} else if((ch & 0xF8) == 0xF0) {
				return 4;
			} else {
				std::wcout << L"String: Invalid UTF8 Character (" << (int)ch << ")." << std::endl;
				return 1;
			}
		}

		int GetUTF8RequiredCharSize(char32_t ch)
		{
			if((ch & 0xFFFFFF80) == 0) {
				return 1;
			} else if((ch & 0xFFFFF800) == 0) {
				return 2;
			} else if((ch & 0xFFFF0000) == 0) {
				return 3;
			} else if((ch & 0xFFE00000) == 0) {
				return 4;
			} else {
				std::wcout << L"String: Unsupported character in UTF8 (" << (int)ch << ")." << std::endl;
				return 1;
			}
		}

		void InsertCharInUTF8(U8String& str, char32_t ch)
		{
			int chSize = GetUTF8RequiredCharSize(ch);
			char first, second, third, fourth;

			switch(chSize) {
				case 1:
					str.push_back((char)ch);
					break;

				case 2:
					second = 0x80 | (ch & 0x3F);
					ch >>= 6;
					first = 0xC0 | (char)ch;

					str.push_back(first);
					str.push_back(second);
					break;

				case 3:
					third = 0x80 | (ch & 0x3F);
					ch >>= 6;
					second = 0x80 | (ch & 0x3F);
					ch >>= 6;
					first = 0xE0 | (char)ch;

					str.push_back(first);
					str.push_back(second);
					str.push_back(third);
					break;

				case 4:
					fourth = 0x80 | (ch & 0x3F);
					ch >>= 6;
					third = 0x80 | (ch & 0x3F);
					ch >>= 6;
					second = 0x80 | (ch & 0x3F);
					ch >>= 6;
					first = 0xF0 | (char)ch;

					str.push_back(first);
					str.push_back(second);
					str.push_back(third);
					str.push_back(fourth);
					break;
			}
		}

		char32_t GetUTF16CharAndMove(U16String::const_iterator& iter)
		{
			char32_t res = 0;

			char16_t ch = *iter;
			int chSize = GetUTF16CharSize(iter);

			switch(chSize) {
				case 1:
					res = ch;
					++iter;
					break;

				case 2:
					char16_t high = ch;
					ch = *(++iter);
					char16_t low = ch;
					++iter;

					res = (((high & 0x3FF) << 10) | (low & 0x3FF)) + 0x10000;
					break;
			}

			return res;
		}
		int GetUTF16CharSize(U16String::const_iterator iter)
		{
			char16_t ch = *iter;

			if((ch & 0xFC00) == 0xD800) {
				return 2;
			} else if((ch & 0xFC00) == 0xD800) {
				std::wcout << L"String: Invalid UTF16 Character (" << ch << "). It is low surrogates." << std::endl;
				return 1;
			} else {
				return 1;
			}
		}
		int GetUTF16RequiredCharSize(char32_t ch)
		{
			if((ch & 0xFFFF0000) == 0) {
				return 1;
			} else {
				return 2;
			}
		}
		void InsertCharInUTF16(U16String& str, char32_t ch)
		{
			int chSize = GetUTF16RequiredCharSize(ch);

			switch(chSize) {
				case 1:
					str.push_back((char16_t)ch);
					break;

				case 2:
					ch -= 0x10000;
					char16_t high = 0xD800 | (char16_t)(ch >> 10);
					char16_t low = 0xDC00 | (ch & 0x3FF);

					str.push_back(high);
					str.push_back(low);
					break;
			}
		}
	} // namespace internal
} // namespace cube

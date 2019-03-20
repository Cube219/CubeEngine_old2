#pragma once

#include <EASTL/string.h>
#include "format.h"

#define STR_UTF16

namespace cube
{
	// Helper class to reference both c-style string and cpp string
	// for using c-style string finally
	// Reference BasicCStringRef in fmt library (made by Victor Zverovich)
	// (https://github.com/fmtlib/fmt)
	template <typename C, typename Str>
	class BaseCStringRef
	{
	public:
		BaseCStringRef(const C* cStr) : 
			mStr(cStr)
		{}
		BaseCStringRef(const Str& cppStr) :
			mStr(cppStr.c_str())
		{}

		const C* c_str() const { return mStr; }

	private:
		const C* mStr;
	};

	// Helper class to reference both c-style string and cpp string
	// for using cpp string finally
	// Reference BasicStringRef in fmt library (made by Victor Zverovich)
	// (https://github.com/fmtlib/fmt)
	template <typename C, typename Str>
	class BaseStringRef
	{
	public:
		BaseStringRef(const C* cStr) : 
			mStr(cStr), mStrPtr(&mStr)
		{}
		BaseStringRef(const Str& cppStr) :
			mStrPtr(&cppStr)
		{}

		const Str& GetString() const { return *mStrPtr; }
		const C* c_str() const { return mStrPtr->c_str(); }

	private:
		const Str* mStrPtr;
		const Str mStr; // Use when using constructor of C-style string
	};

	using U8Character = char;
	using U8String = eastl::basic_string<U8Character>;
	using U8CStringRef = BaseCStringRef<U8Character, U8String>;
	using U8StringRef = BaseStringRef<U8Character, U8String>;

	using U16Character = char16_t;
	using U16String = eastl::basic_string<U16Character>;
	using U16CStringRef = BaseCStringRef<U16Character, U16String>;
	using U16StringRef = BaseStringRef<U16Character, U16String>;

	using U32Character = char32_t;
	using U32String = eastl::basic_string<U32Character>;
	using U32CStringRef = BaseCStringRef<U32Character, U32String>;
	using U32StringRef = BaseStringRef<U32Character, U32String>;

	inline void StringMoveNext(U8String::iterator& iter, size_t offset)
	{
		while(offset > 0) {
			char ch = *iter;
			if((unsigned char)ch <= 127)
				iter += 1;
			else if((ch & 0xE0) == 0xC0)
				iter += 2;
			else if((ch & 0xF0) == 0xE0)
				iter += 3;
			else if((ch & 0xF8) == 0xF0)
				iter += 4;
			else
				throw;

			offset--;
		}
	}
	inline void StringMovePrev(U8String::iterator& iter, size_t offset)
	{
		while(offset > 0) {
			while(1) {
				iter--;

				char ch = *iter;
				if((ch & 0xC0) != 0x80)
					break;
			}

			offset--;
		}
	}

	inline void StringMoveNext(U16String::iterator& iter, size_t offset)
	{
		while(offset > 0) {
			char16_t ch = *iter;

			if((ch & 0xF800) == 0xD800)
				iter += 2;
			else
				iter += 1;

			offset--;
		}
	}
	inline void StringMovePrev(U16String::iterator& iter, size_t offset)
	{
		while(offset > 0) {
			iter--;

			char16_t ch = *iter;
			if((ch & 0xF800) == 0xD800)
				iter--;

			offset--;
		}
	}

	inline void StringMoveNext(U32String::iterator& iter, size_t offset)
	{
		iter += offset;
	}
	inline void StringMovePrev(U32String::iterator& iter, size_t offset)
	{
		iter -= offset;
	}

	eastl::string ToASCIIString(const U8String& str);
	eastl::string ToASCIIString(const U16String& str);
	eastl::string ToASCIIString(const U32String& str);

	U8String ToU8StringFromASCII(const eastl::string& str);
	U8String ToU8String(const U16String& str);
	U8String ToU8String(const U32String& str);

	U16String ToU16StringFromASCII(const eastl::string& str);
	U16String ToU16String(const U8String& str);
	U16String ToU16String(const U32String& str);

	U32String ToU32StringFromASCII(const eastl::string& str);
	U32String ToU32String(const U8String& str);
	U32String ToU32String(const U16String& str);

	namespace internal
	{
		// Helper functions
		char32_t GetUTF8CharAndMove(U8String::const_iterator& iter);
		int GetUTF8CharSize(U8String::const_iterator iter);
		int GetUTF8RequiredCharSize(char32_t ch);
		void InsertCharInUTF8(U8String& str, char32_t ch);

		char32_t GetUTF16CharAndMove(U16String::const_iterator& iter);
		int GetUTF16CharSize(U16String::const_iterator iter);
		int GetUTF16RequiredCharSize(char32_t ch);
		void InsertCharInUTF16(U16String& str, char32_t ch);
	} // namespace internal

#if defined (STR_UTF8)

	using Character = U8Character;
	using String = U8String;
	using CStringRef = U8CStringRef;
	using StringRef = U8StringRef;
	#define CUBE_T(text) u8 ## text
	inline String ToStringFromASCII(const eastl::string& str) { return ToU8StringFromASCII(str); }
	inline String ToString(const U8String& str) { return str; }
	inline String ToString(const U16String& str) { return ToU8String(str); }
	inline String ToString(const U32String& str) { return ToU8String(str); }

#elif defined (STR_UTF16)

	using Character = U16Character;
	using String = U16String;
	using CStringRef = U16CStringRef;
	using StringRef = U16StringRef;
	#define CUBE_T(text) u ## text
	inline String ToStringFromASCII(const eastl::string& str) { return ToU16StringFromASCII(str); }
	inline String ToString(const U8String& str) { return ToU16String(str); }
	inline String ToString(const U16String& str) { return str; }
	inline String ToString(const U32String& str) { return ToU16String(str); }

#elif defined (STR_UTF32)

	using Character = U32Character;
	using String = U32String;
	using CStringRef = U32CStringRef;
	using StringRef = U32StringRef;
	#define CUBE_T(text) U ## text
	inline String ToStringFromASCII(const eastl::string& str) { return ToU32StringFromASCII(str); }
	inline String ToString(const U8String& str) { return ToU32String(str); }
	inline String ToString(const U16String& str) { return ToU32String(str); }
	inline String ToString(const U32String& str) { return str; }

#else
	#error You must define one of string type
#endif
} // namespace cube

// Formatting each strings
namespace fmt
{
	namespace internal
	{
		// U8String
		inline void format_arg(fmt::BasicFormatter<char16_t>& f, const char16_t* format_str, const cube::U8String& s)
		{
			f.writer().write(cube::ToU16String(s));
		}
		inline void format_arg(fmt::BasicFormatter<char32_t>& f, const char32_t* format_str, const cube::U8String& s)
		{
			f.writer().write(cube::ToU32String(s));
		}

		// U16String
		inline void format_arg(fmt::BasicFormatter<char>& f, const char* format_str, const cube::U16String& s)
		{
			f.writer().write(cube::ToU8String(s));
		}
		inline void format_arg(fmt::BasicFormatter<char16_t>& f, const char16_t* format_str, const cube::U16String& s)
		{
			f.writer().write(s);
		}
		inline void format_arg(fmt::BasicFormatter<char32_t>& f, const char32_t* format_str, const cube::U16String& s)
		{
			f.writer().write(cube::ToU32String(s));
		}

		// U32String
		inline void format_arg(fmt::BasicFormatter<char>& f, const char* format_str, const cube::U32String& s)
		{
			f.writer().write(cube::ToU8String(s));
		}
		inline void format_arg(fmt::BasicFormatter<char16_t>& f, const char16_t* format_str, const cube::U32String& s)
		{
			f.writer().write(cube::ToU16String(s));
		}
		inline void format_arg(fmt::BasicFormatter<char32_t>& f, const char32_t* format_str, const cube::U32String& s)
		{
			f.writer().write(s);
		}
	} // namespace internal
} // namespace fmt

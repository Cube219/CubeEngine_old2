#pragma once

#include <EASTL/string.h>
#include "fmt/core.h"

#define STR_UTF16

namespace cube
{
	template <typename C>
	class BaseStringView
	{
	public:
		using iterator = const C*;

	public:
		BaseStringView(const C* cStr) :
			mStr(cStr), mSize(std::char_traits<C>::length(cStr))
		{}

		template <typename Allocator>
		BaseStringView(const eastl::basic_string<C, Allocator>& cppStr) :
			mStr(cppStr.c_str()), mSize(cppStr.size())
		{}

		operator fmt::v5::basic_string_view<C>() const { return fmt::v5::basic_string_view<C>(mStr, mSize); }

		const C* data() const { return mStr; }
		size_t size() const { return mSize; }
		iterator begin() const { return mStr; }
		iterator end() const { return mStr + mSize; }

		int compare(BaseStringView other) const
		{
			size_t compareSize = mSize < other.mSize ? mSize : other.mSize;
			int res = std::char_traits<C>::compare(mStr, other.mStr, compareSize);

			if(res == 0) {
				if(mSize < other.mSize)
					res = -1;
				else if(mSize > other.mSize)
					res = 1;
			}

			return res;
		}

		friend bool operator==(BaseStringView lhs, BaseStringView rhs) {
			return lhs.compare(rhs) == 0;
		}
		friend bool operator!=(BaseStringView lhs, BaseStringView rhs) {
			return lhs.compare(rhs) != 0;
		}
		friend bool operator<(BaseStringView lhs, BaseStringView rhs) {
			return lhs.compare(rhs) < 0;
		}
		friend bool operator<=(BaseStringView lhs, BaseStringView rhs) {
			return lhs.compare(rhs) <= 0;
		}
		friend bool operator>(BaseStringView lhs, BaseStringView rhs) {
			return lhs.compare(rhs) > 0;
		}
		friend bool operator>=(BaseStringView lhs, BaseStringView rhs) {
			return lhs.compare(rhs) >= 0;
		}

	private:
		const C* mStr;
		size_t mSize;
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
	using U8StringView = BaseStringView<U8Character>;
	using U8StringRef = BaseStringRef<U8Character, U8String>;

	using U16Character = char16_t;
	using U16String = eastl::basic_string<U16Character>;
	using U16StringView = BaseStringView<U16Character>;
	using U16StringRef = BaseStringRef<U16Character, U16String>;

	using U32Character = char32_t;
	using U32String = eastl::basic_string<U32Character>;
	using U32StringView= BaseStringView<U32Character>;
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

	// TODO: 모든 Custom Allocator에 대해서 변환 가능하게 만들기(typename 2개 이용)
	inline U8String ToU8String(U8StringRef str) { return str.GetString(); }
	U8String ToU8String(U16StringView str);
	U8String ToU8String(U32StringView str);

	U16String ToU16String(U8StringView str);
	inline U16String ToU16String(U16StringRef str) { return str.GetString(); }
	U16String ToU16String(U32StringView str);

	U32String ToU32String(U8StringView str);
	U32String ToU32String(U16StringView str);
	inline U32String ToU32String(U32StringRef str) { return str.GetString(); }

	namespace internal
	{
		// Helper functions
		char32_t GetUTF8CharAndMove(U8StringView::iterator& iter);
		int GetUTF8CharSize(U8StringView::iterator iter);
		int GetUTF8RequiredCharSize(char32_t ch);
		void InsertCharInUTF8(U8String& str, char32_t ch);

		char32_t GetUTF16CharAndMove(U16StringView::iterator& iter);
		int GetUTF16CharSize(U16StringView::iterator iter);
		int GetUTF16RequiredCharSize(char32_t ch);
		void InsertCharInUTF16(U16String& str, char32_t ch);
	} // namespace internal

#if defined (STR_UTF8)

	using Character = U8Character;
	using String = U8String;
	using StringView = U8StringView;
	using StringRef = U8StringRef;
	#define CUBE_T(text) u8 ## text
	inline String ToString(U8StringRef str) { return str.GetString(); }
	inline String ToString(U16StringView str) { return ToU8String(str); }
	inline String ToString(U32StringView str) { return ToU8String(str); }

#elif defined (STR_UTF16)

	using Character = U16Character;
	using String = U16String;
	using StringView = U16StringView;
	using StringRef = U16StringRef;
	#define CUBE_T(text) u ## text
	inline String ToString(U8StringView str) { return ToU16String(str); }
	inline String ToString(U16StringRef str) { return str.GetString(); }
	inline String ToString(U32StringView str) { return ToU16String(str); }

#elif defined (STR_UTF32)

	using Character = U32Character;
	using String = U32String;
	using StringView = U32StringView;
	using StringRef = U32StringRef;
	#define CUBE_T(text) U ## text
	inline String ToString(U8StringView str) { return ToU32String(str); }
	inline String ToString(U16StringView str) { return ToU32String(str); }
	inline String ToString(U32StringRef str) { return str.GetString; }

#else
	#error You must define one of string type
#endif
} // namespace cube

﻿#pragma once

#include "fmt/format.h"
#include "String.h"

namespace cube
{
	// TODO: 임시로 저장하는 곳
	//       차후 TempAllocator를 쓰면 String이 소멸자를 호출해도 DiscardAllocations()를 쓰기 전까지 살아있으므로
	//       차후 지울 예정

	template <typename Char, typename T>
	inline typename std::enable_if<std::is_same<Char, char>::value, U8String&>::type
		GetTempString(const T& value)
	{
		static U8String tempU8String[10];
		static int tempU8StringIndex = 0;

		tempU8StringIndex %= 10;
		tempU8String[tempU8StringIndex++] = ToU8String(value);

		return tempU8String[tempU8StringIndex-1];
	}

	template <typename Char, typename T>
	inline typename std::enable_if<std::is_same<Char, char16_t>::value, U16String&>::type
		GetTempString(const T& value)
	{
		static U16String tempU16String[10];
		static int tempU16StringIndex = 0;

		tempU16StringIndex %= 10;
		tempU16String[tempU16StringIndex++] = ToU16String(value);

		return tempU16String[tempU16StringIndex - 1];
	}

	template <typename Char, typename T>
	inline typename std::enable_if<std::is_same<Char, char32_t>::value, U32String&>::type
		GetTempString(const T& value)
	{
		static U32String tempU32String[10];
		static int tempU32StringIndex = 0;

		tempU32StringIndex %= 10;
		tempU32String[tempU32StringIndex++] = ToU32String(value);

		return tempU32String[tempU32StringIndex - 1];
	}

	template <typename Char>
	struct GetStringType
	{
	};

	template <>
	struct GetStringType<const char*>
	{
		using type = char;
	};
	template <>
	struct GetStringType<const char16_t*>
	{
		using type = char16_t;
	};
	template <>
	struct GetStringType<const char32_t*>
	{
		using type = char32_t;
	};

	template <>
	struct GetStringType<std::basic_string<char>>
	{
		using type = char;
	};
	template <>
	struct GetStringType<std::basic_string<char16_t>>
	{
		using type = char16_t;
	};
	template <>
	struct GetStringType<std::basic_string<char32_t>>
	{
		using type = char32_t;
	};

#define IS_SAME_STR_TYPE(S, T) (std::is_same<S, typename GetStringType<T>::type>::value)

	// Not a string
	template <typename S, typename T>
	inline typename std::enable_if<!fmt::v5::internal::is_string<T>::value, const T&>::type
		convert_string(const T& value)
	{
		return value;
	}

	// Same string type
	template <typename S, typename T>
	inline typename std::enable_if<
		fmt::v5::internal::is_string<T>::value &&
		IS_SAME_STR_TYPE(S, T), const T&>::type
		convert_string(const T& value)
	{
		return value;
	}

	// Different string type
	template <typename S, typename T>
	inline typename std::enable_if<
		fmt::v5::internal::is_string<T>::value&&
		!IS_SAME_STR_TYPE(S, T), fmt::v5::basic_string_view<S>>::type
		convert_string(const T& value)
	{
		auto& tempStr = GetTempString<S>(value);
		return tempStr;
	}

	template <typename S, typename ...Args>
	inline std::basic_string<typename fmt::v5::char_t<S>::type> Format(const S& format_str, const Args& ...args)
	{
		return fmt::format(format_str, convert_string<typename fmt::v5::char_t<S>::type>(args)...);
	}
} // namespace cube
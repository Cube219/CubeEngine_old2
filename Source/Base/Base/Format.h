﻿#pragma once

#include "fmt/format.h"
#include "BaseTypes.h"
#include "String.h"

#include <iostream>

namespace cube
{
	namespace format
	{
		namespace internal
		{
			constexpr Uint64 blockSize = 256 * 1024; // 256KiB
			struct FormatMemoryBlock
			{
				FormatMemoryBlock()
				{
					startPtr = block;
					currentPtr = startPtr;
				}

				char block[blockSize];
				void* startPtr;
				void* currentPtr;
			};

			void* Allocate(size_t n);
			void* Allocate(size_t n, size_t alignment);

			void DiscardAllocations();
		} // namespace internal
	} // namespace format

	template <typename T>
	class FormatAllocator
	{
	public:
		using value_type = T;

		FormatAllocator(const char* pName = nullptr) {}
		~FormatAllocator() {}

		T* allocate(size_t n, int flags = 0)
		{
			return (T*)format::internal::Allocate(sizeof(T) * n);
		}

		T* allocate(size_t n, size_t alignment, size_t offset, int flags = 0)
		{
			return (T*)format::internal::Allocate(sizeof(T) * n, alignment);
		}

		void deallocate(void* p, size_t n)
		{
			// Do nothing
		}
	};

	template <typename T>
	using FormatString = eastl::basic_string<T, FormatAllocator<char>>;

#define IS_SAME_STR_TYPE(S, T) (std::is_same<S, typename decltype(fmt::v5::to_string_view(fmt::v5::internal::declval<T>()))::value_type>::value)

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
		fmt::v5::internal::is_string<T>::value &&
		!IS_SAME_STR_TYPE(S, T), fmt::v5::basic_string_view<S>>::type
		convert_string(const T& value)
	{
		// temp will be deallocated at the end of the function.
		// But in FormatAllocator, its allocations still alive before
		// calling DiscardAllocations function.
		// So, its string view is validate out of this function.
		FormatString<S> temp;
		temp.set_capacity(16); // For disabling SSO(Short string optimization)
		String_ConvertAndAppend(temp, fmt::to_string_view(value));

		fmt::v5::basic_string_view<S> view(temp.data(), temp.size());
		return view;
	}

	template <typename Char>
	using custom_memory_buffer = fmt::basic_memory_buffer<Char, fmt::inline_buffer_size, FormatAllocator<Char>>;

	template <typename Char, typename StringAllocator>
	inline eastl::basic_string<Char, StringAllocator> vformat_custom(
		fmt::basic_string_view<Char> format_str,
		fmt::basic_format_args<typename fmt::buffer_context<Char>::type> args) {

		custom_memory_buffer<Char> buffer;
		fmt::internal::vformat_to(buffer, format_str, args);

		eastl::basic_string<Char, StringAllocator> res(buffer.data(), buffer.size());

		format::internal::DiscardAllocations();

		return res;
	}

	template <typename S, typename StringAllocator, typename ...Args>
	inline eastl::basic_string<typename fmt::v5::char_t<S>::type, StringAllocator> cube_format(const S& format_str, const Args& ...args)
	{
		using Char = typename fmt::v5::char_t<S>::type;

		return vformat_custom<Char, StringAllocator>(
			fmt::to_string_view(format_str),
			*fmt::internal::checked_args<S, Args...>(format_str, args...));
	}

	// Formats arguments and returns a basic string
	template <typename S, typename ...Args>
	inline eastl::basic_string<typename fmt::v5::char_t<S>::type> Format(const S& format_str, const Args& ...args)
	{
		using Char = typename fmt::v5::char_t<S>::type;

		return cube_format<S, EASTLAllocatorType>(format_str, convert_string<Char>(args)...);
	}

	// Formats arguments and returns a custom allocator string
	// ex)
	//     Format_CustomString<CustomAllocator>("Test {0}", 123);
	template <typename StringAllocator, typename S, typename ...Args>
	inline eastl::basic_string<typename fmt::v5::char_t<S>::type, StringAllocator> Format_CustomString(const S& format_str, const Args& ...args)
	{
		using Char = typename fmt::v5::char_t<S>::type;

		return cube_format<S, StringAllocator>(format_str, convert_string<Char>(args)...);
	}
} // namespace cube

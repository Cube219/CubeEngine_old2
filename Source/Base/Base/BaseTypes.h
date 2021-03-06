#pragma once

#include <stdint.h>

using Int8 = int8_t;
using Int16 = int16_t;
using Int32 = int32_t;
using Int64 = int64_t;

using Uint8 = uint8_t;
static constexpr Uint8 Uint8InvalidValue = (Uint8)(-1);
using Uint16 = uint16_t;
static constexpr Uint16 Uint16InvalidValue = (Uint16)(-1);
using Uint32 = uint32_t;
static constexpr Uint32 Uint32InvalidValue = (Uint32)(-1);
using Uint64 = uint64_t;
static constexpr Uint64 Uint64InvalidValue = (Uint64)(-1);

using SizeType = size_t;

// After Visual Studio 15.8, to use extended alignment, you should define it.
// This will occur when you make shared_ptr with the type of alignment over alignof(max_align_t) using std::make_shared
// (https://developercommunity.visualstudio.com/content/problem/274945/stdmake-shared-is-not-honouring-alignment-of-a.html)
#define _ENABLE_EXTENDED_ALIGNED_STORAGE
#include <memory>

#include <utility>
// TODO: Release일 때 전부 RCast로
#define DCast(T) dynamic_cast<T>

#define RCast(T) reinterpret_cast<T>

#define SCast(T) static_cast<T>

#define CCast(T) const_cast<T>

template <typename T>
using SPtr = std::shared_ptr<T>;

template <typename T>
using UPtr = std::unique_ptr<T>;

template <typename T>
using WPtr = std::weak_ptr<T>;

#define DPCast(T) std::dynamic_pointer_cast<T>

#define SPCast(T) std::static_pointer_cast<T>

#define CPCast(T) std::const_pointer_cast<T>

// TODO: 다른 곳으로 옮기기
enum class MouseButtonType
{
	Left, Right, Middle
};

#include "String.h"

// If you add more STL container, you also have to add the container each custom allocators (FrameAllocator)
#include <EASTL/vector.h>
template <typename T>
using Vector = eastl::vector<T>;

#include <EASTL/array.h>
template <typename Type, size_t Size>
using Array = eastl::array<Type, Size>;

#include <EASTL/map.h>
template <typename Key, typename Value>
using Map = eastl::map<Key, Value>;

template <typename Key, typename Value>
using MultiMap = eastl::multimap<Key, Value>;

#include <EASTL/hash_map.h>
template <typename Key, typename Value>
using HashMap = eastl::hash_map<Key, Value>;

// Deprecated. Use Flags class in Flags.h
#define SET_ENUM_AS_FLAGS(type) \
	inline type operator |(type a, type b)\
	{\
		return static_cast<type>(static_cast<int>(a) | static_cast<int>(b));\
	}\
	inline type operator &(type a, type b)\
	{\
		return static_cast<type>(static_cast<int>(a) & static_cast<int>(b));\
	}\
	inline type& operator |=(type& a, type b)\
	{\
		return a = a | b;\
	}\
	inline type& operator &=(type& a, type b)\
	{\
		return a = a & b;\
	}

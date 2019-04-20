#pragma once

#include "../EngineCoreHeader.h"

#include "Base/Json.h"

namespace cube
{
	ENGINE_CORE_EXPORT FrameAllocator& GetFrameAllocator();

	class ENGINE_CORE_EXPORT FrameAllocator
	{
	private:
		class MemoryBlock
		{
		public:
			MemoryBlock(Uint64 size);
			~MemoryBlock();

			MemoryBlock(const MemoryBlock& other) = delete;
			MemoryBlock& operator=(const MemoryBlock& rhs) = delete;
			
			MemoryBlock(MemoryBlock&& other) : 
				mSize(other.mSize),
				mStartPtr(other.mStartPtr),
				mCurrentPtr(other.mCurrentPtr)
			{
				other.mSize = 0;
				other.mStartPtr = nullptr;
				other.mCurrentPtr = nullptr;
			}
			MemoryBlock& operator=(MemoryBlock&& rhs)
			{
				mSize = rhs.mSize;
				mStartPtr = rhs.mStartPtr;
				mCurrentPtr = rhs.mCurrentPtr;

				rhs.mSize = 0;
				rhs.mStartPtr = nullptr;
				rhs.mCurrentPtr = nullptr;

				return *this;
			}

			void* Allocate(Uint64 size);
			void* AllocateAligned(Uint64 size, Uint64 alignment);

			void DiscardAllocations();

		private:
			Uint64 mSize;

			void* mStartPtr;
			void* mCurrentPtr;
		};

	public:
		FrameAllocator();
		~FrameAllocator();

		FrameAllocator(const FrameAllocator& other) = delete;
		FrameAllocator& operator=(const FrameAllocator& rhs) = delete;

		void Initialize(const char* debugName, Uint64 blockSize = 1 * 1024 * 1024); // 1 MiB
		void ShutDown();

		void* Allocate(Uint64 size);
		void Free(void* ptr);

		void* AllocateAligned(Uint64 size, Uint64 alignment);
		void FreeAligned(void* ptr);

		void DiscardAllocations();

	private:
		void AllocateAdditioanlBlock(Uint64 size);

		Uint64 mBlockSize;
		MemoryBlock mMemoryBlock;

		Vector<MemoryBlock> mAdditionalMemBlocks;

#ifdef _DEBUG
		Uint64 mAllocatedSize = 0;
		const char* mDebugName;
#endif // _DEBUG

		/////////////////////////
		// Class for RapidJson //
		/////////////////////////
	public:
		class RapidJsonAllocator
		{
		public:
			RapidJsonAllocator() :
				mAllocator(GetFrameAllocator())
			{}
			RapidJsonAllocator(FrameAllocator& allocator) :
				mAllocator(allocator)
			{}

#ifdef _DEBUG
			// In debug mode, Free() is needed because it checks total allocated size.
			static const bool kNeedFree = true;

			void* Malloc(size_t size)
			{
				// Add an address of the allocator at prefix
				// bacause Free() is static.
				void* ptr = mAllocator.Allocate(size + sizeof(SizeType*));

				SizeType* storedAddress = (SizeType*)ptr;
				*storedAddress = (SizeType)&mAllocator;

				return (Uint8*)ptr + sizeof(SizeType*);
			}

			void* Realloc(void* originalPtr, size_t originalSize, size_t newSize)
			{
				if(originalPtr != nullptr)
					Free(originalPtr);

				if(newSize == 0)
					return nullptr;

				return Malloc(newSize);
			}

			static void Free(void *ptr)
			{
				if(ptr == nullptr)
					return;

				Uint8* realPtr = (Uint8*)ptr - sizeof(SizeType*);

				FrameAllocator* allocator = (FrameAllocator*)(*(SizeType*)realPtr);
				allocator->Free(realPtr);
			}
#else // _DEBUG
			static const bool kNeedFree = false;

			void* Malloc(size_t size)
			{
				return mAllocator.Allocate(size);
			}

			void* Realloc(void* originalPtr, size_t originalSize, size_t newSize)
			{
				if(newSize == 0)
					return nullptr;

				return Malloc(newSize);
			}

			static void Free(void *ptr)
			{
				// Do nothing
			}
#endif // _DEBUG

		private:
			FrameAllocator& mAllocator;
		};

		/////////////////////
		// Class for EASTL //
		/////////////////////
	public:
		class EASTLAllocator
		{
		public:
			EASTLAllocator(const char* pName = nullptr) :
				mpAllocator(&GetFrameAllocator())
			{}
			EASTLAllocator(FrameAllocator& allocator) :
				mpAllocator(&allocator)
			{}

			void* allocate(size_t n, int flags = 0)
			{
				return mpAllocator->AllocateAligned(n, EASTL_ALLOCATOR_MIN_ALIGNMENT);
			}

			void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0)
			{
				return mpAllocator->AllocateAligned(n, alignment);
			}

			void deallocate(void* p, size_t n)
			{
				mpAllocator->FreeAligned(p);
			}

		private:
			FrameAllocator* mpAllocator;
		};

	public:
		RapidJsonAllocator& GetRapidJsonAllocator() { return mRapidJsonAllocator; }
		EASTLAllocator& GetEASTLAllocator() { return mEASTLAllocator; }

	private:
		RapidJsonAllocator mRapidJsonAllocator;
		EASTLAllocator mEASTLAllocator;
	};

	// Define strings with frame allocator
	using FrameU8String = eastl::basic_string<U8Character, FrameAllocator::EASTLAllocator>;
	using FrameU16String = eastl::basic_string<U16Character, FrameAllocator::EASTLAllocator>;
	using FrameU32String = eastl::basic_string<U32Character, FrameAllocator::EASTLAllocator>;

#if defined (STR_UTF8)
	using FrameString = FrameU8String;
#elif defined (STR_UTF16)
	using FrameString = FrameU16String;
#elif defined (STR_UTF32)
	using FrameString = FrameU32String;
#endif

	// Define STL containers with frame allocator
	template <typename T>
	using FrameVector = eastl::vector<T, FrameAllocator::EASTLAllocator>;

	template <typename Key, typename Value>
	using FrameMap = eastl::map<Key, Value, eastl::less<Key>, FrameAllocator::EASTLAllocator>;

	template <typename Key, typename Value>
	using FrameMultiMap = eastl::multimap<Key, Value, eastl::less<Key>, FrameAllocator::EASTLAllocator>;

	template <typename Key, typename Value>
	using FrameHashMap = eastl::hash_map<Key, Value, eastl::hash<Key>, eastl::equal_to<Key>, FrameAllocator::EASTLAllocator>;

	// Define Json with frame allocator
	using FrameJson = rapidjson::GenericDocument<rapidjson::UTF8<>, FrameAllocator::RapidJsonAllocator>;
	using FrameJsonValue = rapidjson::GenericValue<rapidjson::UTF8<>, FrameAllocator::RapidJsonAllocator>;
} // namespace cube

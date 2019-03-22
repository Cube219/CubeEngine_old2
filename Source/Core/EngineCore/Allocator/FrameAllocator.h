#pragma once

#include "../EngineCoreHeader.h"

namespace cube
{
	class FrameAllocator
	{
	private:
		class MemoryBlock
		{
		public:
			MemoryBlock(Uint64 size);
			~MemoryBlock();

			void* Allocate(Uint64 size);
			void* AllocateAligned(Uint64 size, Uint64 alignment);

			void DiscardAllocations();

		private:
			Uint64 mSize;

			void* mStartPtr;
			void* mCurrentPtr;
		};

	public:
		FrameAllocator(Uint64 blockSize = 5 * 1024 * 1024); // 0 MiB
		~FrameAllocator();

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
#endif // _DEBUG
	};
} // namespace cube

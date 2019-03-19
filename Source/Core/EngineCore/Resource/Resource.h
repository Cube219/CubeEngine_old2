#pragma once

#include "../EngineCoreHeader.h"

#include "../Thread/MutexLock.h"
#include "FileSystem.h"

namespace cube
{
	class ENGINE_CORE_EXPORT Resource
	{
	public:
		Resource():
			mRefCount(0)
		{ }
		virtual ~Resource(){ }

	protected:
		friend class ResourceManager;
		template <typename T>
		friend class ResourcePointer;

		Atomic<Uint32> mRefCount;
	};
} // namespace cube

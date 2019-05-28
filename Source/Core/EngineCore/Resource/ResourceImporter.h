#pragma once

#include "../EngineCoreHeader.h"

#include "../Allocator/FrameAllocator.h"

#include "FileSystem.h"

namespace cube
{
	class ENGINE_CORE_EXPORT ResourceImporter
	{
	public:
		ResourceImporter() {}
		virtual ~ResourceImporter() {}

		virtual Resource* Import(SPtr<platform::File>& file, const FrameJsonValue& info) = 0;

		StringView GetName() const { return mName; }

	protected:
		String mName;
	};
} // namespace cube

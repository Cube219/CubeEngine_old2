#pragma once

#include "../EngineCoreHeader.h"

#include <rapidjson/document.h>

#include "FileSystem.h"

namespace cube
{
	class ENGINE_CORE_EXPORT ResourceImporter
	{
	public:
		ResourceImporter() {}
		virtual ~ResourceImporter() {}

		virtual Resource* Import(SPtr<platform::File>& file, const rapidjson::Value& info) = 0;

		const String& GetName() const { return mName; }

	protected:
		String mName;
	};
} // namespace cube

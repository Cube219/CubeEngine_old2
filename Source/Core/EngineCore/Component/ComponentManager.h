﻿#pragma once

#include "../EngineCoreHeader.h"

#include <functional>
#include "../Handler.h"
#include "../Allocator/FrameAllocator.h"

namespace cube
{
	class ENGINE_CORE_EXPORT ComponentManager
	{
	public:
		ComponentManager();
		~ComponentManager();

		ComponentManager(const ComponentManager& other) = delete;
		ComponentManager& operator=(const ComponentManager& rhs) = delete;
		ComponentManager(ComponentManager&& other) = delete;
		ComponentManager& operator=(ComponentManager&& rhs) = delete;

		void Initialize();
		void ShutDown();

		template <typename T>
		void RegisterComponent()
		{
			StringView name = T::GetName();

			CheckIfComponentExisted(name);

			mComponentCreators[name.data()] = [this]() {
				mComponents.push_back(std::make_unique<T>());
				return mComponentTable.CreateNewHandler<T>(DCast(T*)(mComponents.back().get()));
			};
		}

		HComponent CreateComponent(StringView name);

	private:
		void CheckIfComponentExisted(StringView name);

		HashMap<String, std::function<HComponent()>> mComponentCreators;
		HandlerTable mComponentTable;
		Vector<UPtr<Component>> mComponents;
	};
} // namespace cube

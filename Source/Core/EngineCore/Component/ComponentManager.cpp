#include "ComponentManager.h"

#include "../Assertion.h"
#include "Component.h"

namespace cube
{
	ComponentManager::ComponentManager() : 
		mComponentTable(30)
	{
	}

	ComponentManager::~ComponentManager()
	{
	}

	void ComponentManager::Initialize()
	{
	}

	void ComponentManager::ShutDown()
	{
		mComponentTable.ReleaseAll();
		mComponents.clear();
	}

	HComponent ComponentManager::CreateComponent(StringView name)
	{
		auto findIter = mComponentCreators.find_as(name.data());
		CHECK(findIter != mComponentCreators.end(), "Failed to create component '{0}'. It hasn't been registered", name);

		return findIter->second();
	}

	void ComponentManager::CheckIfComponentExisted(StringView name)
	{
		auto findIter = mComponentCreators.find_as(name.data());
		if(findIter != mComponentCreators.end()) {
			CUBE_LOG(LogType::Error, "Component '{0}' is already registered", name);
			return;
		}
	}
} // namespace cube

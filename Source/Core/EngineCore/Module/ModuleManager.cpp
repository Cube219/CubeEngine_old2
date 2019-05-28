#include "ModuleManager.h"

#include "Platform.h"

#include "../EngineCore.h"
#include "BaseModule.h"
#include "../Thread/ThreadManager.h"
#include "../Thread/Thread.h"

#include "../LogWriter.h"
#include "../Assertion.h"

namespace cube
{
	void ModuleManager::Initialize()
	{
	}

	void ModuleManager::ShutDown()
	{
		for(auto& m : mModules) {
			m.module->Destroy();
		}

		// mModuleLoopup must be freed before mModules
		// Because the module cannot be freed after its DLib has been unloaded
		mModuleLookup.clear();
		mModules.clear();
	}

	void ModuleManager::LoadModule(StringView moduleName)
	{
		auto temp = mModuleLookup.find_as(moduleName.data());
		if(temp != mModuleLookup.end()) {
			CUBE_LOG(LogType::Error, "Already module name '{0}' loaded.", moduleName);
			return;
		}

		ModuleNode node;
			
		node.moduleDLib = platform::Platform::LoadDLib(moduleName);
			
		using CreateModuleFunction = BaseModule* (*)();

		auto createModuleFunction = RCast(CreateModuleFunction)(node.moduleDLib->GetFunction(CUBE_T("CreateModule")));
		node.module = SPtr<BaseModule>(createModuleFunction());

		mModules.push_back(node);
		mModuleLookup[moduleName.data()] = node.module;
	}

	void ModuleManager::InitModules()
	{
		for(auto& m : mModules) {
			m.module->Init(&ECore());
		}
	}

	SPtr<BaseModule> ModuleManager::GetModule(StringView name)
	{
		auto temp = mModuleLookup.find_as(name.data());
		CHECK(temp != mModuleLookup.end(), "Failed to find module '{0}'.", name);

		return temp->second;
	}

	void ModuleManager::UpdateAllModules(float dt)
	{
		for(auto iter = mModules.begin(); iter != mModules.end(); iter++) {
			iter->module->Update(dt);
		}
	}
} // namespace cube

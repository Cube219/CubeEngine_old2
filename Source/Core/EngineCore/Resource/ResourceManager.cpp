#include "ResourceManager.h"

#include "../Assertion.h"
#include "Resource.h"
#include "FileSystem.h"
#include "../Allocator/FrameAllocator.h"

namespace cube
{
	void ResourceManager::Initialize()
	{
	}

	void ResourceManager::ShutDown()
	{
		UnloadUnusedResources();

		for(auto& res : mLoadedResources) {
			delete res.second;
		}
		mLoadedResources.clear();
		mImporters.clear();
	}

	void ResourceManager::RegisterImporter(UPtr<ResourceImporter> importer)
	{
		mImporters.push_back(std::move(importer));
	}
		
	RPtr<Resource> ResourceManager::LoadResource(StringView path)
	{
		using namespace platform;

		// Check if the resource is already loaded
		{
			Lock lock(mLoadedResourcesMutex);

			auto findIter = mLoadedResources.find(path.data()); // TODO: UUID로 바꾸기
			if(findIter != mLoadedResources.end()) {
				RPtr<Resource> resPtr(findIter->second);
				return resPtr;
			}
		}

		// Get a metadata
		String metaPath(path.data(), path.size());
		metaPath.append(CUBE_T(".cmeta"));
		SPtr<File> metaFile = platform::FileSystem::OpenFile(metaPath, FileAccessModeFlag::Read);

		Uint64 size = metaFile->GetFileSize();
		char* metaString = (char*)malloc(size + 1);

		Uint64 readSize = 0;
		metaFile->Read(metaString, size, readSize);
		metaString[size] = '\0';

		FrameJson metaJson;
		metaJson.Parse(metaString);
		free(metaString);
			
		U8String importerNameU8 = metaJson["importer_name"].GetString();
		String importerName;
		String_ConvertAndAppend(importerName, importerNameU8);

		Resource* loadedRes = nullptr;
		// Find importer to import the resource
		bool isFindImporter = false;
		for(auto& importer : mImporters) {
			if(importer->GetName() == StringView(importerName)) {
				SPtr<File> resFile = platform::FileSystem::OpenFile(path, FileAccessModeFlag::Read);
				const FrameJsonValue& info = metaJson["info"];

				loadedRes = importer->Import(resFile, info);
				isFindImporter = true;
				break;
			}
		}

		CHECK(isFindImporter == true, "Failed to find the importer '{0}'.", importerName);
		CHECK(loadedRes != nullptr, "Failed to load the resource.");

		{
			Lock lock(mLoadedResourcesMutex);

			mLoadedResources[path.data()] = loadedRes; // TODO: UUID로 바꾸기
		}
		return RPtr<Resource>(loadedRes);
	}
		
	void ResourceManager::ReleaseResource(const RPtr<Resource>& res)
	{
		Lock lock(mLoadedResourcesMutex);
	}

	void ResourceManager::UnloadUnusedResources()
	{
		Lock lock(mLoadedResourcesMutex);

		Vector<String> resToUnload;
		for(auto iter = mLoadedResources.begin(); iter != mLoadedResources.end(); iter++) {
			if(iter->second->mRefCount == 0) {
				resToUnload.push_back(iter->first);
			}
		}

		for(auto& res : resToUnload) {
			delete mLoadedResources[res];
			mLoadedResources.erase(res);
		}
	}
} // namespace cube

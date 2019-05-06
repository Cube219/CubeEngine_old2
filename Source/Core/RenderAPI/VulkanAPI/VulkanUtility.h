#pragma once

#include <vulkan/vulkan.h>

#include "Base/String.h"
#include "Base/Format.h"

#ifdef _DEBUG

namespace cube
{
	namespace render
	{
		void PrintVkFailImpl(VkResult res, const char* fileName, int lineNum, StringView msg);

		template <typename ...Args>
		void PrintVkFail(VkResult res, const char* fileName, int lineNum, StringView msg, Args&&... args)
		{
			PrintVkFailImpl(res, fileName, lineNum, Format(msg.data(), std::forward<Args>(args)...));
		}

		const Character* GetVkResult(VkResult res);
	} // namespace render
} // namespace cube

#define CHECK_VK(res, msg, ...) if(res != VK_SUCCESS) cube::render::PrintVkFail(res, __FILE__, __LINE__, CUBE_T(msg), ##__VA_ARGS__)

#else // _DEBUG

#define CHECK_VK(res, msg, ...)

#endif // _DEBUG

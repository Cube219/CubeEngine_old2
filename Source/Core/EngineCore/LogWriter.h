#pragma once

#ifdef ENGINE_CORE_EXPORTS
#define ENGINE_CORE_EXPORT __declspec(dllexport) 
#else // ENGINE_CORE_EXPORTS
#define ENGINE_CORE_EXPORT __declspec(dllimport) 
#endif // ENGINE_CORE_EXPORTS

#include "Base/BaseTypes.h"
#include "Base/Format.h"
#include "Allocator/FrameAllocator.h"

namespace cube
{
	enum class LogType
	{
		Info, Warning, Error
	};

	class ENGINE_CORE_EXPORT LogWriter
	{
	public:

		template <typename ...Args>
		static void WriteLog(LogType type, const char* fileName, int lineNum, StringView msg, Args&&... args)
		{
			FrameString str = FrameFormat(msg.data(), std::forward<Args>(args)...);
			WriteLogImpl(type, fileName, lineNum, str);
		}

	private:
		LogWriter() = delete;
		LogWriter(const LogWriter& other) = delete;
		LogWriter& operator=(const LogWriter& other) = delete;
			
		friend class EngineCore;

		template <typename ...Args>
		friend void AssertionFailed(const char* fileName, int lineNum, StringView msg, Args&&... args);

		static void WriteLogImpl(LogType type, const char* fileName, int lineNum, StringView msg);
		static void WriteLogImpl(StringView log);

		// Only can access to the friend class (EngineCore)
		static void Init();

		static const char* SplitFileName(const char* fullPath);

	};
} // namespace cube

#define CUBE_LOG(type, msg, ...) cube::LogWriter::WriteLog(type, __FILE__, __LINE__, CUBE_T(msg), ##__VA_ARGS__)

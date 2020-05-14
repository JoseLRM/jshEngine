#pragma once

#include "types.h"

namespace jsh {
	class Mesh;
}

namespace jshDebug {

	namespace _internal {
		void Initialize();
		void Close();
		void RegisterLog();
	}

	bool InDebugMode();

	void ShowConsole();
	void HideConsole();

	void Log(const char* s, ...);
	void LogE(const char* s, ...);
	void LogI(const char* s, ...);
	void LogW(const char* s, ...);
	void LogSeparator();
	namespace _internal {
		void LogA(const char* s, uint32 line, const char* file);
		void LogF(const char* s, uint32 line, const char* file, ...);
	}

#ifdef JSH_IMGUI
	void ShowImGuiWindow();
#endif

}

#define jshFatalError(desc, ...) jshDebug::_internal::LogF(desc, __LINE__, __FILE__, __VA_ARGS__)
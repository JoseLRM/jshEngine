#include "common.h"

#include "WinLib.h"

#include "TaskSystem.h"
#include "State.h"
#include "Renderer.h"
#include "Graphics.h"
#include "Archive.h"

#include <stdio.h>
#ifdef JSH_CONSOLE
#include <iostream>
#endif

namespace jshDebug {

	std::string g_LogFilePath = "logs/";
	jsh::TxtFile g_LogFile;
	std::mutex g_LogMutex;

	namespace _internal {
		void Initialize()
		{
#ifdef JSH_CONSOLE
			ShowConsole();
#else
			HideConsole();
#endif 

			std::string dateStr = jshTimer::GetDate().ToString();
			g_LogFilePath += dateStr + ".log";
			g_LogFile << jshEngine::GetName() << '\n';
		}

		void Close()
		{
			RegisterLog();
		}

		void RegisterLog()
		{
			g_LogFile.SaveFile(g_LogFilePath.c_str(), false);
			g_LogFile.str(std::string());
		}
	}

#ifdef JSH_ENGINE
#ifdef JSH_DEBUG
	bool InDebugMode() { return true; }
#else
	bool InDebugMode() { return false; }
#endif
#endif

	void ShowConsole()
	{
		HWND console = GetConsoleWindow();
		ShowWindow(console, SW_SHOWDEFAULT);
	}

	void HideConsole()
	{
		HWND console = GetConsoleWindow();
		ShowWindow(console, SW_HIDE);
	}

	std::string _LogToString(const char* s, va_list args)
	{
		char commandLine[200];
		vsnprintf(&commandLine[0], 200, s, args);
		return std::string(commandLine);
	}
	std::string LogToString(const char* s, ...)
	{
		va_list args;
		va_start(args, s);
		std::string str = std::move(_LogToString(s, args));
		va_end(args);
		return str;
	}
	inline void _Log(const char* s0, const char* s1, bool notifyTime = true)
	{
		std::lock_guard<std::mutex>lock(g_LogMutex);
#ifdef JSH_CONSOLE
		std::cout << s0;
		std::cout << s1 << std::endl;
#endif

		if (notifyTime) {
			jsh::Date date = jshTimer::GetDate();
			g_LogFile << '[' << uint32(date.hour) << ':' << uint32(date.minute) << ':' << uint32(date.second) << ']' << s0 << s1 << '\n';
		}
		else {
			g_LogFile << s0 << s1 << '\n';
		}
		if (g_LogFile.str().size() >= 1000) _internal::RegisterLog();
	}
	void Log(const char* s, ...)
	{
		va_list args;
		va_start(args, s);
		_Log("", _LogToString(s, args).c_str());
		va_end(s);
	}
	void LogE(const char* s, ...)
	{
		va_list args;
		va_start(args, s);
		_Log("[ERROR] ", _LogToString(s, args).c_str());
		va_end(s);
		_internal::RegisterLog();
	}
	void LogI(const char* s, ...)
	{
		va_list args;
		va_start(args, s);
		_Log("[INFO] ", _LogToString(s, args).c_str());
		va_end(s);
	}
	void LogW(const char* s, ...)
	{
		va_list args;
		va_start(args, s);
		_Log("[WARNING] ", _LogToString(s, args).c_str());
		va_end(s);
	}
	void LogSeparator()
	{
		_Log("", "---------------------------------", false);
	}
	void _internal::LogA(const char* s, uint32 line, const char* file)
	{
		std::string logStr = LogToString("Assertion failed '%s', file: '%s', line: %u", s, file, line);
		_Log("[ASSERT] ", logStr.c_str());
		jshEngine::Close();

		std::wstring logWStr;
		logWStr.resize(logStr.size());
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, logStr.c_str(), int(logStr.size()), &logWStr[0], int(logStr.size()));

		std::wstring pathWStr;
		pathWStr.resize(g_LogFilePath.size());
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, g_LogFilePath.c_str(), int(g_LogFilePath.size()), &pathWStr[0], int(g_LogFilePath.size()));

		MessageBox(NULL, std::wstring(logWStr + L". See " + pathWStr +  L" for more details.").c_str(), L"Assertion Failed", MB_ICONERROR | MB_OK);
		jshEngine::Exit(1);
	}
	void _internal::LogF(const char* s, uint32 line, const char* file, ...)
	{
		std::string logStr = LogToString("File '%s', line: %u", file, line);

		va_list args;
		va_start(args, file);
		std::string logStr2 = _LogToString(s, args);
		va_end(args);

		Log("[FATAL ERROR] '%s'. %s", logStr2.c_str(), logStr.c_str());

		jshEngine::Close();

		std::wstring logWStr;
		std::wstring logWStr2;

		logWStr.resize(logStr.size());
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, logStr.c_str(), int(logStr.size()), &logWStr[0], int(logStr.size()));

		logWStr2.resize(logStr2.size());
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, logStr2.c_str(), int(logStr2.size()), &logWStr2[0], int(logStr2.size()));

		std::wstring pathWStr;
		pathWStr.resize(g_LogFilePath.size());
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, g_LogFilePath.c_str(), g_LogFilePath.size(), &pathWStr[0], g_LogFilePath.size());

		MessageBox(NULL, std::wstring(L"'" + logWStr2 + L"'. " + logWStr + L". See " + pathWStr + L" for more details.").c_str(), L"Fatal Error!!", MB_ICONERROR | MB_OK);
		jshEngine::Exit(1);
	}

#ifdef JSH_IMGUI
	bool g_ShowTaskSystem = false;
	bool g_ShowEntities = false;
	bool g_ShowSystems = false;
	bool g_ShowMeshes = false;
	bool g_ShowRawData = false;
	bool g_ShowMaterials = false;
	bool g_ShowShaders = false;
	bool g_ShowTextures = false;
	bool g_ShowRenderer = false;
	bool g_ShowGraphics = false;
	bool g_ShowImGuiDemo = false;

	void ShowImGuiWindow()
	{
		// MAIN MENU BAR
		if (ImGui::BeginMainMenuBar()) {

			if (ImGui::Button("Task System")) g_ShowTaskSystem = !g_ShowTaskSystem;
			if (ImGui::Button("Entities")) g_ShowEntities = !g_ShowEntities;
			if (ImGui::Button("Systems")) g_ShowSystems = !g_ShowSystems;

			if (ImGui::BeginMenu("Gfx Objects")) {
				if (ImGui::Button("Meshes")) g_ShowMeshes = !g_ShowMeshes;
				if (ImGui::Button("RawData")) g_ShowRawData = !g_ShowRawData;
				if (ImGui::Button("Materials")) g_ShowMaterials = !g_ShowMaterials;
				if (ImGui::Button("Shaders")) g_ShowShaders = !g_ShowShaders;
				if (ImGui::Button("Textures")) g_ShowTextures = !g_ShowTextures;

				ImGui::EndMenu();
			}
			if (ImGui::Button("Renderer")) g_ShowRenderer = !g_ShowRenderer;
			if (ImGui::Button("Graphics")) g_ShowGraphics = !g_ShowGraphics;

			if (ImGui::Button("ImGui Demo")) g_ShowImGuiDemo = !g_ShowImGuiDemo;

			uint32 FPS = jshEngine::GetFPS();
			ImVec4 col = { 0.f, 1.f, 0.f, 1.f };
			if (FPS < 50u) col = { 1.f, 0.f, 0.f, 1.f };
			ImGui::TextColored(col, ("FPS: " + std::to_string(jshEngine::GetFPS())).c_str());

			ImGui::EndMainMenuBar();
		}

		// Systems
		if (g_ShowTaskSystem) g_ShowTaskSystem = jshTask::ShowImGuiWindow();
		if (g_ShowEntities) g_ShowEntities = jshScene::ShowImGuiEntityWindow();
		if (g_ShowSystems) g_ShowSystems = jshScene::ShowImGuiSystemsWindow();
		if (g_ShowMeshes) g_ShowMeshes = jshGraphics::ShowMeshImGuiWindow();
		if (g_ShowRawData) g_ShowRawData = jshGraphics::ShowRawDataImGuiWindow();
		if (g_ShowMaterials) g_ShowMaterials = jshGraphics::ShowMaterialImGuiWindow();
		if (g_ShowShaders) g_ShowShaders = jshGraphics::ShowShaderImGuiWindow();
		if (g_ShowTextures) g_ShowTextures = jshGraphics::ShowTextureImGuiWindow();
		if (g_ShowRenderer) {
			if (ImGui::Begin("Renderer")) {
				jshEngine::GetRenderer()->ShowInfo();
			}
			ImGui::End();
		}
		if (g_ShowGraphics) g_ShowGraphics = jshGraphics::ShowImGuiWindow();
		if (g_ShowImGuiDemo) ImGui::ShowDemoWindow(&g_ShowImGuiDemo);
	}

#endif

}
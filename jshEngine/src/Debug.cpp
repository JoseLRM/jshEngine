#include "Debug.h"

#include "graphics/WinLib.h"

#include "TaskSystem.h"
#include "Application.h"
#include "State.h"

namespace jshDebug {

	bool Initialize()
	{
#ifdef JSH_CONSOLE
		ShowConsole();
#else
		HideConsole();
#endif 
		return true;
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

	void ShowOkWindow(const wchar* const description, uint8 level) {
		if (level == 0)
			MessageBox(NULL, description, L"Just information", MB_ICONINFORMATION | MB_OK);
		else if (level == 1)
			MessageBox(NULL, description, L"Warning!", MB_ICONWARNING | MB_OK);
		else
			MessageBox(NULL, description, L"Error!!", MB_ICONERROR | MB_OK);
	}

	void ShowOkWindow(const wchar* const title, const wchar* const description, uint8 level)
	{
		if (level == 0)
			MessageBox(NULL, description, title, MB_ICONINFORMATION | MB_OK);
		else if (level == 1)
			MessageBox(NULL, description, title, MB_ICONWARNING | MB_OK);
		else
			MessageBox(NULL, description, title, MB_ICONERROR | MB_OK);
	}

#ifdef JSH_IMGUI
	bool g_ShowTaskSystem = false;
	bool g_ShowState = false;
	bool g_ShowImGuiDemo = false;

	void ShowImGuiWindow()
	{
		// MAIN MENU BAR
		if (ImGui::BeginMainMenuBar()) {

			if (ImGui::BeginMenu("Debug")) {

				if (ImGui::Button("Task System")) g_ShowTaskSystem = true;
				if (ImGui::Button("State")) g_ShowState = true;

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("ImGui Demo")) {

				g_ShowImGuiDemo = true;

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		// Systems
		if (g_ShowTaskSystem) {
			g_ShowTaskSystem = jshTask::ShowImGuiWindow();
		}
		if (g_ShowState) {
			jsh::State* state = jshApplication::GetCurrentState();
			if (state) g_ShowState = jshApplication::GetCurrentState()->ShowImGuiWindow();
			else g_ShowState = false;
		}
		if(g_ShowImGuiDemo) ImGui::ShowDemoWindow(&g_ShowImGuiDemo);
	}

#endif

}
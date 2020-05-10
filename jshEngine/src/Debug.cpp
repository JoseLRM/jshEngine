#include "Debug.h"

#include "WinLib.h"

#include "TaskSystem.h"
#include "Engine.h"
#include "State.h"
#include "Renderer.h"
#include "Graphics.h"

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
#include "Application.h"

#include "graphics/WinLib.h"
#include "graphics/DirectX11Lib.h"

#include "graphics/Window.h"
#include "TaskSystem.h"
#include "Timer.h"
#include "graphics/Graphics.h"
#include "Debug.h"
#include "State.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

using namespace jsh;

namespace jshApplication {

	bool g_Initialized = false;
	bool g_Closed = false;
	State* g_CurrentState = nullptr;

	struct Controller {
		Controller()
		{
			
		}
		~Controller()
		{
			Close();
#ifdef JSH_CONSOLE
			system("pause");
#endif 

		}
	};
	Controller controller;

	bool Initialize(State* initialState)
	{
		if (g_Initialized) return false;

		jshTimer::Initialize();

		if (!jshTask::Initialize()) {
			jshLogE("Can't initialize jshTaskSystem");
			return false;
		}

		if (!jshWindow::Initialize()) {
			jshLogE("Can't initialize jshWindow");
			return false;
		}

		if (!jshGraphics::Initialize()) {
			jshLogE("Can't initialize jshGraphics");
			return false;
		}

		if (!jshDebug::Initialize()) return false;

		//im gui initialize
#ifdef JSH_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(jshWindow::GetWindowHandle());
		ImGui_ImplDX11_Init((ID3D11Device*)jshGraphics::GetDevice(), (ID3D11DeviceContext*)jshGraphics::GetContext());
		ImGui::StyleColorsDark();
#endif

		if (initialState) {
			g_CurrentState = initialState;
			initialState->Initialize();
		}

		jshLogI("jshEngine initialized");
		g_Initialized = true;
		return true;
	}

	void Run()
	{
		Initialize();

		Time lastTime = jshTimer::Now();
		Time deltaTime = lastTime;
		Time actualTime = 0.f;

		float fixedUpdateCount = 0.f;

		while (jshWindow::UpdateInput()) {
			actualTime = jshTimer::Now();

			deltaTime = actualTime - lastTime;
			lastTime = actualTime;

			fixedUpdateCount += deltaTime;

			if (g_CurrentState) {
				// update
				g_CurrentState->Update(deltaTime);

				if (fixedUpdateCount >= 0.01666666f) {
					fixedUpdateCount -= 0.01666666f;
					g_CurrentState->FixedUpdate();
				}

				// render
				jshGraphics::Prepare();
				
				// prepare imgui
				jshImGui(ImGui_ImplDX11_NewFrame());
				jshImGui(ImGui_ImplWin32_NewFrame());
				jshImGui(ImGui::NewFrame());
				jshImGui(jshDebug::ShowImGuiWindow());

				g_CurrentState->Render();

				// render imgui
				jshImGui(ImGui::Render());
				jshImGui(ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()));

			}

		}
	}

	bool Close()
	{
		if (g_Closed) return false;
		CloseState();

		if (!jshTask::Close()) return false;
		
		if (!jshWindow::Close()) return false;

		if (!jshGraphics::Close()) return false;

		g_Closed = true;
		return true;
	}

	void LoadState(jsh::State* state)
	{
		if (!g_Initialized) {
			jshDebug::ShowOkWindow(L"Use 'jshApplication::Initialize(new State())' to set the first state", 2);
			return;
		}
		CloseState();
		g_CurrentState = state;
		g_CurrentState->Initialize();
	}

	void CloseState()
	{
		if (g_CurrentState) {
			g_CurrentState->Close();
			delete g_CurrentState;
			g_CurrentState = nullptr;
		}
		jshLogI("jshEngine closed");
	}

	State* GetCurrentState()
	{
		return g_CurrentState;
	}

}
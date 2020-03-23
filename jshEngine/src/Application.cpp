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

	bool initialized = false;
	bool closed = false;
	State* currentState = nullptr;

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
		if (initialized) return false;

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
			currentState = initialState;
			initialState->Initialize();
		}

		jshLogI("jshEngine initialized");
		initialized = true;
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

			if (currentState) {
				// update
				currentState->Update(deltaTime);

				if (fixedUpdateCount >= 0.01666666f) {
					fixedUpdateCount -= 0.01666666f;
					currentState->FixedUpdate();
				}

				// render
				jshGraphics::Prepare();
				
				// prepare imgui
#ifdef JSH_IMGUI
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
#endif

				currentState->Render();

#ifdef JSH_IMGUI
				ImGui::Render();
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

			}

		}
	}

	bool Close()
	{
		if (closed) return false;
		CloseState();

		if (!jshTask::Close()) return false;
		
		if (!jshWindow::Close()) return false;

		if (!jshGraphics::Close()) return false;

		closed = true;
		return true;
	}

	void LoadState(jsh::State* state)
	{
		if (!initialized) {
			jshDebug::ShowOkWindow(L"Use 'jshApplication::Initialize(new State())' to set the first state", 2);
			return;
		}
		CloseState();
		currentState = state;
		currentState->Initialize();
	}

	void CloseState()
	{
		if (currentState) {
			currentState->Close();
			delete currentState;
			currentState = nullptr;
		}
		jshLogI("jshEngine closed");
	}

}
#include "Engine.h"

#include "graphics/Window.h"
#include "TaskSystem.h"
#include "Timer.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer.h"
#include "Debug.h"
#include "State.h"

using namespace jsh;

namespace jshEngine {

#ifdef JSH_CONSOLE
	namespace _DontTouchItxD {
		struct ConsoleController {
			~ConsoleController()
			{
				system("pause");
			}
		};
		ConsoleController pleaseKillMe;
	}
#endif 

	bool g_Initialized = false;
	bool g_Closed = false;
	State* g_CurrentState = nullptr;
	uint32 g_FPS = 0u;
	uint32 g_FixedUpdateFrameRate;
	float g_FixedUpdateDeltaTime;

	bool Initialize(State* initialState)
	{
		if (g_Initialized) return false;
		
		try {
			jshTimer::Initialize();

			if (!jshTask::Initialize()) {
				jshLogE("Can't initialize jshTaskSystem");
				return false;
			}

			SetFixedUpdateFrameRate(60u);

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
			jshImGui(if (!jshGraphics::InitializeImGui()) {
				jshLogE("Cant't initialize ImGui");
				return false;
			});

			jshScene::Initialize();

			if (initialState) {
				g_CurrentState = initialState;
				initialState->Initialize();
			}
			jshLogI("jshEngine initialized");
			g_Initialized = true;
		}
		catch (jsh::Exception e) {
			e.what();
			return false;
		}
		catch (std::exception e) {
			jshLogE(e.what());
			return false;
		}
		catch (...) {
			jshLogE("Unknown error");
			return false;
		}
		return true;
	}

	void Run()
	{
		Initialize();

		try {

			Time lastTime = jshTimer::Now();
			Time deltaTime = lastTime;
			Time actualTime = 0.f;

			const float SHOW_FPS_RATE = 0.1f;
			float dtCount = 0.f;
			float fpsCount = 0u;

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
					jshRenderer::BeginFrame();
					g_CurrentState->Render();
					jshRenderer::EndFrame();

				}

				// FPS count
				dtCount += deltaTime;
				fpsCount++;
				if (dtCount >= SHOW_FPS_RATE) {
					g_FPS = fpsCount / SHOW_FPS_RATE;
					fpsCount = 0.f;
					dtCount -= SHOW_FPS_RATE;
				}
			}
		}
		catch (jsh::Exception e) {
			e.what();
		}
		catch (std::exception e) {
			jshLogE(e.what());
		}
		catch (...) {
			jshLogE("Unknown error");
		}

	}

	bool Close()
	{
		try {
			if (g_Closed) return false;
			CloseState();

			jshScene::Close();

			if (!jshTask::Close()) return false;

			if (!jshWindow::Close()) return false;

			if (!jshGraphics::Close()) return false;

			g_Closed = true;
		}
		catch (jsh::Exception e) {
			e.what();
			return false;
		}
		catch (std::exception e) {
			jshLogE(e.what());
			return false;
		}
		catch (...) {
			jshLogE("Unknown error");
			return false;
		}
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

	uint32 GetFPS()
	{
		return g_FPS;
	}

	void SetFixedUpdateFrameRate(uint32 frameRate)
	{
		g_FixedUpdateFrameRate = frameRate;
		g_FixedUpdateDeltaTime = 1.f / (float)frameRate;
	}
	float GetFixedUpdateDeltaTime()
	{
		return g_FixedUpdateDeltaTime;
	}

}
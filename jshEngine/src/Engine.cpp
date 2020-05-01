#include "Engine.h"

#include "Window.h"
#include "TaskSystem.h"
#include "Timer.h"
#include "Graphics.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
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
	Time g_DeltaTime = 0.f;
	uint32 g_FixedUpdateFrameRate;
	float g_FixedUpdateDeltaTime;

	Renderer* g_pRenderer;

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
			
			if(!g_pRenderer->Initialize()) return false;

			jshLogI("jshEngine initialized");
			g_Initialized = true;
		}
		catch (jsh::Exception e) {
			e.what();
			return false;
		}
		catch (std::exception e) {
			jshLogE("%s", e.what());
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

		try {

			Time lastTime = jshTimer::Now();
			g_DeltaTime = lastTime;
			Time actualTime = 0.f;

			const float SHOW_FPS_RATE = 1.0f;
			float dtCount = 0.f;
			float fpsCount = 0u;

			float fixedUpdateCount = 0.f;

			while (jshWindow::UpdateInput()) {
				actualTime = jshTimer::Now();

				g_DeltaTime = actualTime - lastTime;
				lastTime = actualTime;

				fixedUpdateCount += g_DeltaTime;

				if (g_CurrentState) {
					// update
					g_CurrentState->Update(g_DeltaTime);

					if (fixedUpdateCount >= 0.01666666f) {
						fixedUpdateCount -= 0.01666666f;
						g_CurrentState->FixedUpdate();
					}

					// render
					g_pRenderer->Begin();
					g_CurrentState->Render();
					g_pRenderer->Render();
					g_pRenderer->End();
				}

				// FPS count
				dtCount += g_DeltaTime;
				fpsCount++;
				if (dtCount >= SHOW_FPS_RATE) {
					g_FPS = fpsCount / SHOW_FPS_RATE;
					jshLogln("%u", g_FPS);
					fpsCount = 0.f;
					dtCount -= SHOW_FPS_RATE;
				}
			}
		}
		catch (jsh::Exception e) {
			e.what();
		}
		catch (std::exception e) {
			jshLogE("%s", e.what());
		}
		catch (...) {
			jshLogE("Unknown error");
		}

	}

	bool Close()
	{
		try {
			if (!g_pRenderer->Close()) return false;

			if (g_Closed) return false;
			CloseState();

			jshScene::Close();

			if (!jshTask::Close()) return false;

			if (!jshWindow::Close()) return false;

			if (!jshGraphics::Close()) return false;

			g_Closed = true;
			jshLogI("jshEngine closed");
		}
		catch (jsh::Exception e) {
			e.what();
			return false;
		}
		catch (std::exception e) {
			jshLogE("%s",e.what());
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
	}

	State* GetCurrentState()
	{
		return g_CurrentState;
	}

	uint32 GetFPS()
	{
		return g_FPS;
	}
	float GetDeltaTime()
	{
		return g_DeltaTime;
	}

	bool IsInitialized()
	{
		return g_Initialized;
	}

	// RENDERER
	void SetRenderer(jsh::Renderer* renderer)
	{
		g_pRenderer = renderer;
		if (g_Initialized) g_pRenderer->Initialize();
	}

	jsh::Renderer* GetRenderer()
	{
		return g_pRenderer;
	}

	// FIXED UPDATE METHODS
	void SetFixedUpdateFrameRate(uint32 frameRate)
	{
		g_FixedUpdateFrameRate = frameRate;
		g_FixedUpdateDeltaTime = 1.f / (float)frameRate;
	}
	float GetFixedUpdateDeltaTime()
	{
		return g_FixedUpdateDeltaTime;
	}

	// VERSION
	constexpr uint64 g_MajorVersion = 0u;
	constexpr uint64 g_MinorVersion = 4u;
	constexpr uint64 g_RevisionVersion = 1u;

	uint64 GetMajorVersion()
	{
		return g_MajorVersion;
	}
	uint64 GetMinorVersion()
	{
		return g_MinorVersion;
	}
	uint64 GetRevisionVersion()
	{
		return g_RevisionVersion;
	}

	uint64 GetVersion()
	{
		return g_MajorVersion * 1000000u + g_MinorVersion * 1000u + g_RevisionVersion;
	}
	const char* GetVersionStr()
	{
		const static std::string ss = g_MajorVersion << '.' << g_MinorVersion << '.' << g_RevisionVersion;
		return ss.c_str();
	}
	const wchar* GetVersionStrW()
	{
		const static std::wstring ss = std::to_wstring(g_MajorVersion) + L'.' + std::to_wstring(g_MinorVersion) + L'.' + std::to_wstring(g_RevisionVersion);
		return ss.c_str();
	}

	// PROPERTIES
	const char* GetName()
	{
		const static std::string name = "jshEngine " + std::string(GetVersionStr());
		return name.c_str();
	}
	const wchar* GetNameW()
	{
		const static std::wstring name = L"jshEngine " + std::wstring(GetVersionStrW());
		return name.c_str();
	}

}
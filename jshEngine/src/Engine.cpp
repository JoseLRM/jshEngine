#include "Engine.h"

#include "Window.h"
#include "TaskSystem.h"
#include "Timer.h"
#include "Graphics.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "Debug.h"
#include "State.h"
#include "DefaultLoadState.h"
#include "StateManager.h"

using namespace jsh;
using namespace jsh::_internal;

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
	uint32 g_FPS = 0u;
	Time g_DeltaTime = 0.f;
	uint32 g_FixedUpdateFrameRate;
	float g_FixedUpdateDeltaTime;

	StateManager g_State;

	Renderer* g_pRenderer;

	bool Initialize(jsh::State* state, jsh::State* loadState)
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

			LoadState(state, loadState);
			
			if(g_pRenderer) if(!g_pRenderer->Initialize()) return false;

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

				g_State.Prepare();

				// update
				g_State.Update(g_DeltaTime);

				if (fixedUpdateCount >= 0.01666666f) {
					fixedUpdateCount -= 0.01666666f;
					g_State.FixedUpdate();
				}

				// render
				if (g_pRenderer) {
					g_pRenderer->Begin();
					g_State.Render();
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
			
			g_State.ClearState();

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

	////////////////////////////////////////STATE MANAGEMENT////////////////////////////
	void LoadState(jsh::State* state, jsh::State* loadState)
	{
		g_State.LoadState(state, loadState);
	}

	State* GetCurrentState()
	{
		return g_State.GetCurrentState();
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
	constexpr uint64 g_RevisionVersion = 2u;

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
#include "common.h"

#include "Window.h"
#include "TaskSystem.h"
#include "Timer.h"
#include "Graphics.h"
#include "Renderer3D.h"
#include "State.h"
#include "DefaultLoadState.h"
#include "StateManager.h"
#include "GuiSystem.h"

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

	void BeginUpdate(float dt);
	void EndUpdate(float dt);

	uint32 g_FPS = 0u;
	Time g_DeltaTime = 0.f;
	uint32 g_FixedUpdateFrameRate;
	float g_FixedUpdateDeltaTime;

	StateManager g_State;

	Renderer* g_pRenderer;

	GuiSystem g_GuiSystem;

	enum JSH_ENGINE_STATE : uint8 {
		JSH_ENGINE_STATE_NONE,
		JSH_ENGINE_STATE_INITIALIZING,
		JSH_ENGINE_STATE_INITIALIZED,
		JSH_ENGINE_STATE_RUNNING,
		JSH_ENGINE_STATE_CLOSING,
		JSH_ENGINE_STATE_CLOSED
	};
	JSH_ENGINE_STATE g_EngineState = JSH_ENGINE_STATE_NONE;

	bool Initialize(jsh::State* state, jsh::State* loadState)
	{
		jshTimer::Initialize();
		jshDebug::_internal::Initialize();

		if (g_EngineState != JSH_ENGINE_STATE_NONE) {
			jshDebug::LogW("jshEngine is already initialized");
			return false;
		}

		g_EngineState = JSH_ENGINE_STATE_INITIALIZING;
		
		try {

			if (!jshTask::Initialize()) {
				jshDebug::LogE("Can't initialize jshTaskSystem");
				return false;
			}

			SetFixedUpdateFrameRate(60u);

			if (!jshWindow::Initialize()) {
				jshDebug::LogE("Can't initialize jshWindow");
				return false;
			}

			if (!jshGraphics::Initialize()) {
				jshDebug::LogE("Can't initialize jshGraphics");
				return false;
			}

			//im gui initialize
			jshImGui(if (!jshGraphics::InitializeImGui()) {
				jshDebug::LogE("Cant't initialize ImGui");
				return false;
			});

			SetRenderer(new Renderer3D());

			jshScene::Initialize();

			g_GuiSystem.Initialize();

			LoadState(state, loadState);
			
			if(g_pRenderer) if(!g_pRenderer->Initialize()) return false;

			jshDebug::LogI("jshEngine initialized");
			jshDebug::LogSeparator();
			g_EngineState = JSH_ENGINE_STATE_INITIALIZED;
		}
		catch (std::exception e) {
			jshFatalError("STD Exception: '%s'", e.what());
			return false;
		}
		catch (...) {
			jshFatalError("Unknown error");
			return false;
		}
		return true;
	}

	void Run()
	{
		if (g_EngineState != JSH_ENGINE_STATE_INITIALIZED) {
			jshFatalError("You must initialize jshEngine");
		}

		g_EngineState = JSH_ENGINE_STATE_RUNNING;

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

				if (g_DeltaTime > 0.2f) g_DeltaTime = 0.2f;

				g_State.Prepare();

				// update
				BeginUpdate(g_DeltaTime);
				g_State.Update(g_DeltaTime);

				if (fixedUpdateCount >= 0.01666666f) {
					fixedUpdateCount -= 0.01666666f;
					g_State.FixedUpdate();
				}
				EndUpdate(g_DeltaTime);

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
					g_FPS = uint32(fpsCount / SHOW_FPS_RATE);
					//jshDebug::Log("%u", g_FPS);
					fpsCount = 0.f;
					dtCount -= SHOW_FPS_RATE;
				}
			}
		}
		catch (std::exception e) {
			jshFatalError("STD Exception: '%s'", e.what());
		}
		catch (...) {
			jshFatalError("Unknown error");
		}
	}

	bool Close()
	{
		jshDebug::LogSeparator();

		if (g_EngineState == JSH_ENGINE_STATE_CLOSING || g_EngineState == JSH_ENGINE_STATE_CLOSED) {
			jshDebug::LogW("jshEngine is already closed");
		}
		
		g_EngineState = JSH_ENGINE_STATE_CLOSING;

		try {

			if (g_pRenderer) {
				if (!g_pRenderer->Close()) return false;
				delete g_pRenderer;
			}
			
			g_State.ClearState();

			jshScene::Close();

			if (!jshTask::Close()) return false;

			if (!jshWindow::Close()) return false;

			if (!jshGraphics::Close()) return false;

			jshDebug::LogI("jshEngine closed");
			jshDebug::_internal::Close();
			g_EngineState = JSH_ENGINE_STATE_CLOSED;
		}
		catch (std::exception e) {
			jshFatalError("STD Exception: '%s'", e.what());
			return false;
		}
		catch (...) {
			jshFatalError("Unknown error");
			return false;
		}
		return true;
	}

	void Exit(int code)
	{
		if (g_EngineState != JSH_ENGINE_STATE_CLOSED) {
			if (!Close()) jshDebug::LogE("Can't close jshEngine properly");
		}
		exit(code);
	}

	///////////////////////////////////////UPDATE/////////////////////////////////////

	void BeginUpdate(float dt)
	{
		g_GuiSystem.Update(dt);
	}
	void EndUpdate(float dt)
	{
		// Update Camera matrices
		{
			auto& cameras = jshScene::_internal::GetComponentsList()[CameraComponent::ID];
			for (uint32 i = 0; i < cameras.size(); i += uint32(CameraComponent::SIZE)) {
				CameraComponent* camera = reinterpret_cast<CameraComponent*>(&cameras[i]);
				camera->UpdateMatrices();
			}
		}
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
		return g_EngineState != JSH_ENGINE_STATE_NONE && g_EngineState != JSH_ENGINE_STATE_INITIALIZING;
	}

	// RENDERER
	void SetRenderer(jsh::Renderer* renderer)
	{
		if (g_pRenderer) {
			g_pRenderer->Close();
			delete g_pRenderer;
		}
		g_pRenderer = renderer;
		if (IsInitialized()) g_pRenderer->Initialize();
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
	constexpr uint64 g_MinorVersion = 1u;
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
		const static std::string str = std::string(std::to_string(g_MajorVersion) + '.' + std::to_string(g_MinorVersion) + '.' + std::to_string(g_RevisionVersion));
		return str.c_str();
	}
	const wchar* GetVersionStrW()
	{
		const static std::wstring str = std::wstring(std::to_wstring(g_MajorVersion) + L'.' + std::to_wstring(g_MinorVersion) + L'.' + std::to_wstring(g_RevisionVersion));
		return str.c_str();
	}

	// PROPERTIES
	const char* GetName()
	{
		const static std::string str = std::string("jshEngine " + std::string(GetVersionStr()));
		return str.c_str();
	}
	const wchar* GetNameW()
	{
		const static std::wstring str = std::wstring(L"jshEngine " + std::wstring(GetVersionStrW()));
		return str.c_str();
	}

}
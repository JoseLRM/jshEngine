#include "Application.h"

#include "WinLib.h"

#include "Window.h"
#include "TaskSystem.h"
#include "Timer.h"
#include "Graphics.h"
#include "Debug.h"
#include "State.h"
#include "Input.h"

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

		if (initialState) {
			currentState = initialState;
			initialState->Initialize();
		}

		jshLogI("jshEngine initialized");
		initialized = true;
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
				currentState->Render();
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
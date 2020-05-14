#pragma once

#include "types.h"

namespace jsh {
	class State;
	class Renderer;
}

namespace jshEngine {

	bool Initialize(jsh::State* state, jsh::State* loadState = nullptr);
	void Run();
	bool Close();

	void Exit(int code);

	bool IsInitialized();

	// state management
	void LoadState(jsh::State* state, jsh::State* loadState = nullptr);
	jsh::State* GetCurrentState();

	uint32 GetFPS();
	float GetDeltaTime();

	// renderer
	void SetRenderer(jsh::Renderer* renderer);

	jsh::Renderer* GetRenderer();

	// fixedUpdate methods
	void SetFixedUpdateFrameRate(uint32 frameRate);
	float GetFixedUpdateDeltaTime();

	// version
	uint64 GetMajorVersion();
	uint64 GetMinorVersion();
	uint64 GetRevisionVersion();

	uint64 GetVersion();
	const wchar* GetVersionStrW();
	const char* GetVersionStr();

	// properties
	const wchar* GetNameW();
	const char* GetName();

}
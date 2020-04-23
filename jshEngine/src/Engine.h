#pragma once

#include "common.h"

#include "Scene.h"

namespace jsh {
	class State;
}

namespace jshEngine {

	bool Initialize(jsh::State* initialState = nullptr);
	void Run();
	bool Close();

	void CloseState();
	void LoadState(jsh::State* state);
	jsh::State* GetCurrentState();

	uint32 GetFPS();
	float GetDeltaTime();

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
#pragma once

#include "common.h"

#include "Scene.h"

namespace jsh {
	class State;
	class Renderer;
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

	// renderer
	void SetRenderer(jsh::Renderer* renderer);
	void SetDefaultRenderer2D();
	void SetDefaultRenderer3D();

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
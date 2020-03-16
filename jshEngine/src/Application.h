#pragma once

#include "common.h"

namespace jsh {
	class State;
}

namespace jshApplication {

	bool Initialize(jsh::State* initialState = nullptr);
	void Run();
	bool Close();

	void CloseState();
	void LoadState(jsh::State* state);
}
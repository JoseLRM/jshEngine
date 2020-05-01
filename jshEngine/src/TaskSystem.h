#pragma once

#include "ThreadPrimitives.h"

namespace jsh {

	struct ThreadArgs {
		size_t index;
	};

}

namespace jshTask {

#ifdef JSH_ENGINE
	bool Initialize();
	bool Close();
#endif

	using AsyncTask = std::function<void(jsh::ThreadArgs& args)>;

	void Execute(const jsh::Task& task, jsh::ThreadContext* pContext = nullptr);
	void Execute(jsh::Task* tasks, uint32 count, jsh::ThreadContext* pContext = nullptr);

	void Async(size_t length, uint8 divisions, const AsyncTask& task);

	bool Running();
	bool Running(jsh::ThreadContext* pContext);

	void Wait();
	void Wait(jsh::ThreadContext* pContext);

	uint8 ThreadCount();

#if defined(JSH_ENGINE) && defined(JSH_IMGUI)

	bool ShowImGuiWindow();

#endif

}
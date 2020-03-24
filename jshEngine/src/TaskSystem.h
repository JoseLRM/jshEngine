#pragma once

#include "utils/dataStructures/safe_queue.h"
#include "common.h"
#include <memory>
#include <functional>

namespace jsh {

	typedef std::function<void()> Task;

	struct TaskList {
#ifndef JSH_ENGINE
	private:
#endif
		std::queue<Task> tasks;

	public:
		void Add(const Task& task);
	};

	struct ThreadArgs {
		size_t index = 0u;
	};

}

namespace jshTask {

#ifdef JSH_ENGINE
	bool Initialize();
	bool Close();
#endif

	jsh::TaskList* CreateTaskList();

	void Execute(jsh::TaskList* taskList);
	void Execute(const jsh::Task& task);

	void Async(size_t length, uint8 divisions, const std::function<void(jsh::ThreadArgs& args)>&);

	bool Doing();
	void Wait();

	uint8 ThreadCount();

#if defined(JSH_ENGINE) && defined(JSH_IMGUI)

	bool ShowImGuiWindow();

#endif

}
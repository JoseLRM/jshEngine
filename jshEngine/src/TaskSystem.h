#pragma once

#include "utils/dataStructures/safe_queue.h"
#include <memory>
#include <functional>

namespace jshTask {

	typedef std::function<void()> Task;

	struct TaskList {
#ifndef JSH_ENGINE
	private:
#endif
		std::queue<Task> tasks;

		TaskList();

	public:
		void Add(const Task& task);
	};

	struct ThreadArgs {
		size_t index = 0u;
	};

#ifdef JSH_ENGINE
	bool Initialize();
	bool Close();
#endif

	TaskList* CreateTaskList();

	void Execute(TaskList* taskList);
	void Execute(const Task& task);

	void Async(size_t length, uint8 divisions, const std::function<void(ThreadArgs& args)>&);

	bool Doing(TaskList* taskList = nullptr);
	void Wait(TaskList* taskList = nullptr);

	uint8 ThreadCount();
}
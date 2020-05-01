#include "TaskSystem.h"

#include "Debug.h"

using namespace jsh;

namespace jshTask {

	uint32 g_NumOfThreads;
	ThreadPool g_ThreadPool;

	bool Initialize()
	{
		g_NumOfThreads = std::thread::hardware_concurrency();
		if (g_NumOfThreads <= 1) g_NumOfThreads = 2u;

		g_ThreadPool.Reserve(g_NumOfThreads);

		return true;
	}

	bool Close()
	{
		g_ThreadPool.Stop();

		return true;
	}

	void Execute(const jsh::Task& task, jsh::ThreadContext* pContext)
	{
		g_ThreadPool.Execute(task, pContext);
	}
	void Execute(jsh::Task* tasks, uint32 count, jsh::ThreadContext* pContext)
	{
		g_ThreadPool.Execute(tasks, count, pContext);
	}

	bool Running()
	{
		return g_ThreadPool.Running();
	}
	bool Running(jsh::ThreadContext* pContext)
	{
		return g_ThreadPool.Running(pContext);
	}

	void Wait()
	{
		g_ThreadPool.Wait();
	}
	void Wait(jsh::ThreadContext* pContext)
	{
		g_ThreadPool.Wait(pContext);
	}

	void Async(size_t length, uint8 divisions, const AsyncTask& task)
	{
		Task* tasks = new Task[divisions];

		size_t dividedLength = length / divisions;

		for (uint8 i = 0; i < divisions; ++i) {

			tasks[i] = ([i, dividedLength, divisions, length, task]() {
			
				size_t index = i * dividedLength;
				size_t endIndex = (i == (divisions-1)) ? length : index + dividedLength;
				ThreadArgs args;
				while (index < endIndex) {
					args.index = index;
					task(args);
					++index;
				}
			
			});

		}

		ThreadContext context;
		Execute(tasks, divisions, &context);
		Wait(&context);
		delete[] tasks;
	}

	uint8 ThreadCount() {
		return g_NumOfThreads;
	}

#ifdef JSH_IMGUI

	bool ShowImGuiWindow()
	{
		bool result = true;
		if (ImGui::Begin("Task System")) {
			ImGui::Text(("Thread count: " + std::to_string(g_NumOfThreads)).c_str());

			if (ImGui::Button("Close")) result = false;
		}
		ImGui::End();
		return result;
	}

#endif

}
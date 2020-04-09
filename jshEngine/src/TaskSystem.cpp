#include "TaskSystem.h"

#include "Debug.h"
#include <vector>
#include <atomic>

namespace jsh {

	void TaskList::Add(const Task& task)
	{
		tasks.push(task);
	}

}

using namespace jsh;

namespace jshTask {

	bool g_Running = false;
	uint8 g_NumOfThreads = 0;

	class ThreadPool;
	ThreadPool* g_Pools = nullptr;

	jsh::safe_dynamic_queue<TaskList*> g_TaskLists;
	std::condition_variable g_ConditionVariable;
	std::mutex g_SleepMutex;
	std::mutex g_ExecuteMutex;

	std::atomic<uint32> g_CompletedTasks = 0;
	uint32 g_DoingTasks = 0;

	class ThreadPool {
		TaskList* m_pList = nullptr;
		std::mutex m_Mutex;

		bool GetInternal(Task& task) {
			bool result = false;
			if (m_pList) {
				if (!m_pList->tasks.empty()) {
					task = m_pList->tasks.front();
					m_pList->tasks.pop();
					result = true;
				}
				else {
					delete m_pList;
					m_pList = nullptr;
				}
			}
			if (!result) {
				if (g_TaskLists.pop(m_pList)) {
					result = GetInternal(task);
				}
			}
			return result;
		}

	public:
		ThreadPool() : m_Mutex() {}

		bool Get(Task& task) {
			m_Mutex.lock();
			bool result = GetInternal(task);
			m_Mutex.unlock();
			return result;
		}

		inline TaskList* CurrentTaskList() { return m_pList; }

	};

	bool Initialize()
	{
		g_NumOfThreads = std::thread::hardware_concurrency();
		if (g_NumOfThreads < 1) g_NumOfThreads = 1;

		g_Pools = new ThreadPool[g_NumOfThreads];

		g_Running = true;
		for (uint8 i = 0; i < g_NumOfThreads; ++i) {

			std::thread worker([i]() {

				uint8 ID = i;
				Task currentTask;
			
				while (g_Running) {
					
					bool doing = true;
					while (doing) {
						doing = false;
						if (g_Pools[ID].Get(currentTask)) {
							doing = true;
							currentTask();
							g_CompletedTasks.fetch_add(1u);
						}
						else {
							for (uint8 i = 0; i < g_NumOfThreads; ++i) {
								if (i == ID) continue;
								if (g_Pools[i].Get(currentTask)) {
									doing = true;
									currentTask();
									g_CompletedTasks.fetch_add(1u);
								}
							}
						}
					}

					std::unique_lock<std::mutex> lock(g_SleepMutex);
					g_ConditionVariable.wait(lock);
				}

				jshLogI("Thread %u closed", ID);
			
			});

			worker.detach();

		}

		return true;
	}

	bool Close()
	{
		Wait();
		g_Running = false;
		g_ConditionVariable.notify_all();
		return true;
	}

	TaskList* CreateTaskList()
	{
		TaskList* taskList = new TaskList();
		return taskList;
	}

	void Execute(TaskList* taskList)
	{
		g_ExecuteMutex.lock();
		uint32 size = uint32(taskList->tasks.size());
		g_DoingTasks += size;
		g_TaskLists.push(taskList);
		if (size == 1) g_ConditionVariable.notify_one();
		else g_ConditionVariable.notify_all();
		g_ExecuteMutex.unlock();
	}

	void Execute(const Task& task)
	{
		TaskList* taskList = CreateTaskList();
		taskList->Add(task);
		Execute(taskList);
	}

	void Async(size_t length, uint8 divisions, const std::function<void(ThreadArgs& args)>& fn)
	{
		TaskList* list = CreateTaskList();

		size_t dividedLength = length / divisions;

		for (uint8 i = 0; i < divisions; ++i) {

			list->Add([i, dividedLength, divisions, length, fn]() {
			
				size_t index = i * dividedLength;
				size_t endIndex = (i == (divisions-1)) ? length : index + dividedLength;
				ThreadArgs args;
				while (index < endIndex) {
					args.index = index;
					fn(args);
					++index;
				}
			
			});

		}

		Execute(list);
	}

	bool Doing()
	{
		return g_DoingTasks != g_CompletedTasks.load();
	}

	void Wait()
	{
		while (Doing()) {
			g_ConditionVariable.notify_one();
			std::this_thread::yield();
		}
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
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

	bool running = false;
	uint8 numOfThreads = 0;

	class ThreadPool;
	ThreadPool* pools = nullptr;

	jsh::safe_dynamic_queue<TaskList*> taskLists;
	std::condition_variable cv;
	std::mutex sleepMutex;
	std::mutex executeMutex;

	std::atomic<uint32> completedTasks = 0;
	uint32 doingTasks = 0;

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
				if (taskLists.pop(m_pList)) {
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
		numOfThreads = std::thread::hardware_concurrency();
		if (numOfThreads < 1) numOfThreads = 1;

		pools = new ThreadPool[numOfThreads];

		running = true;
		for (uint8 i = 0; i < numOfThreads; ++i) {

			std::thread worker([i]() {

				uint8 ID = i;
				Task currentTask;
			
				while (running) {
					
					bool doing = true;
					while (doing) {
						doing = false;
						if (pools[ID].Get(currentTask)) {
							doing = true;
							currentTask();
							completedTasks.fetch_add(1u);
						}
						else {
							for (uint8 i = 0; i < numOfThreads; ++i) {
								if (i == ID) continue;
								if (pools[i].Get(currentTask)) {
									doing = true;
									currentTask();
									completedTasks.fetch_add(1u);
								}
							}
						}
					}

					std::unique_lock<std::mutex> lock(sleepMutex);
					cv.wait(lock);
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
		running = false;
		cv.notify_all();
		return true;
	}

	TaskList* CreateTaskList()
	{
		TaskList* taskList = new TaskList();
		return taskList;
	}

	void Execute(TaskList* taskList)
	{
		executeMutex.lock();
		doingTasks += taskList->tasks.size();
		taskLists.push(taskList);
		cv.notify_all();
		executeMutex.unlock();
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
		return doingTasks != completedTasks.load();
	}

	void Wait()
	{

		Task currentTask;

		// do all the lists
		while (Doing()) {
			for (uint8 i = 0; i < numOfThreads; ++i) {
				while (pools[i].Get(currentTask)) {
					currentTask();
					completedTasks.fetch_add(1u);
				}
				std::this_thread::yield();
				cv.notify_one();
			}
		}
		
	}

	uint8 ThreadCount() {
		return numOfThreads;
	}

}
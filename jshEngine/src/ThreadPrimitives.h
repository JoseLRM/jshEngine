#pragma once

#include "common.h"
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>
#include <atomic>

// temp
#include "Debug.h"

namespace jsh {

	using Task = std::function<void()>;

	class ThreadPool;

	class ThreadContext {
		std::atomic<uint32> executedTasks = 0u;
		uint32 taskCount;
	public:
		friend ThreadPool;
	};

	class ThreadPool {
		std::vector<std::thread> m_Threads;

		std::condition_variable m_ConditionVar;
		std::mutex m_SleepMutex;
		std::mutex m_ExecuteMutex;

		std::atomic<uint64> m_TaskExecutedCount = 0u;
		uint64 m_TaskInQueueCount = 0u;

		std::queue<std::pair<Task, ThreadContext*>> m_Tasks;

		bool m_Running = true;

	public:
		ThreadPool() {}

		inline void Reserve(uint32 cant) noexcept
		{
			m_Threads.reserve(cant);
			uint32 lastID = m_Threads.size();
			for (uint32 i = 0; i < cant; ++i) {
				uint32 ID = i + lastID;
				m_Threads.emplace_back([this, ID]() {
					Task task;
					ThreadContext* pContext;

					while (m_Running) {
						// sleep - critical section
						{
							std::unique_lock<std::mutex> lock(m_SleepMutex);
							m_ConditionVar.wait(lock, [&]() { return !m_Running || !m_Tasks.empty(); });

							if (m_Tasks.empty()) continue;

							task = std::move(m_Tasks.front()).first;
							pContext = m_Tasks.front().second;
							m_Tasks.pop();
						}

						// execution
						task();
						if (pContext) pContext->executedTasks.fetch_add(1);
						m_TaskExecutedCount.fetch_add(1);
					}
				});
			}
		}

		inline void Execute(const Task& task, ThreadContext* context = nullptr) noexcept
		{
			std::lock_guard<std::mutex> lock(m_ExecuteMutex);

			if (context) context->taskCount = 1;
			
			m_Tasks.emplace(std::make_pair(std::move(task), context));
			m_TaskInQueueCount++;
			m_ConditionVar.notify_one();
		}

		inline void Execute(Task* tasks, uint32 count, ThreadContext* context = nullptr) noexcept
		{
			std::lock_guard<std::mutex> lock(m_ExecuteMutex);

			if (context) context->taskCount = count;

			for (uint32 i = 0; i < count; ++i) {
				m_Tasks.emplace(std::make_pair(std::move(tasks[i]), context));
			}

			m_TaskInQueueCount += count;
			m_ConditionVar.notify_all();
		}

		inline bool Running() const noexcept
		{
			return m_TaskExecutedCount.load() != m_TaskInQueueCount;
		}

		inline bool Running(ThreadContext* pContext) const noexcept
		{
			return pContext->executedTasks.load() != pContext->taskCount;
		}

		inline void Wait(ThreadContext* pContext) noexcept
		{
			while (Running(pContext)) {
				std::this_thread::yield();
				m_ConditionVar.notify_all();
			}
		}

		inline void Wait() noexcept
		{
			while (Running()) {
				std::this_thread::yield();
				m_ConditionVar.notify_all();
			}
		}

		inline void Stop() noexcept
		{
			m_Running = false;
			m_ConditionVar.notify_all();

			for (uint32 i = 0; i < m_Threads.size(); ++i) m_Threads[i].join();
		}

	};

}
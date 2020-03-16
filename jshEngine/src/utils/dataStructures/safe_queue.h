#pragma once

#include "common.h"
#include <mutex>
#include <queue>

namespace jsh {

	template<typename T, size_t size>
	class safe_queue {
		T m_Data[size];

		size_t m_Head = 0u;
		size_t m_Tail = 0u;

		std::mutex m_Mutex;

	public:
		safe_queue() : m_Mutex() {}

		bool push(const T& obj)
		{
			bool result = false;
			m_Mutex.lock();

			size_t nextHead = (m_Head + 1) % size;

			if (nextHead != m_Tail) {
				m_Data[m_Head] = obj;
				m_Head = nextHead;
				result = true;
			}

			m_Mutex.unlock();
			return result;
		}
		bool pop(T& obj)
		{
			bool result = false;
			m_Mutex.lock();

			if (m_Tail != m_Head) {
				obj = m_Data[m_Tail];
				m_Tail = (m_Tail + 1) % size;
				result = true;
			}

			m_Mutex.unlock();
			return result;
		}

	};

	template<typename T>
	class safe_dynamic_queue {
		std::queue<T> m_Queue;
		std::mutex m_Mutex;

	public:
		safe_dynamic_queue() : m_Mutex() {}

		void push(const T& obj)
		{
			m_Mutex.lock();
			m_Queue.push(obj);
			m_Mutex.unlock();
		}
		bool pop(T& obj)
		{
			m_Mutex.lock();
			bool result = !m_Queue.empty();
			if (result) {
				obj = m_Queue.front();
				m_Queue.pop();
			}
			m_Mutex.unlock();
			return result;
		}

	};

}
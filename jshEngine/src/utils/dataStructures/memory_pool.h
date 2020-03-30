#pragma once

#include "vector.h"
#include <mutex>

namespace jsh {

	template<typename T>
	class memory_pool {

		vector<T> m_Data;
		vector<uint32> m_RemovedList;
		mutable std::mutex m_Mutex;

	public:
		memory_pool() {}

		uint32 push(const T& t = T()) noexcept
		{
			uint32 id;
			if (m_RemovedList.empty()) {
				id = m_Data.size();
				m_Data.push_back(t, 10);
			}
			else {
				id = m_RemovedList.back();
				m_RemovedList.pop_back();
			}
			return id + 1;
		}

		void remove(uint32 id) noexcept
		{
			m_RemovedList.push_back(id-1, 10);
		}

		inline bool empty() const noexcept { return (m_Data.size() - m_RemovedList.size()) == 0; }
		inline T& pop() noexcept {
			uint32 id;
			while (true) {
				m_Data.pop_back();
				id = m_Data.size();
				
				bool find = true;
				for (uint32 i = 0; i < m_RemovedList.size(); ++i) {
					if (m_RemovedList[i] == id) {
						find = false;
						break;
					}
				}
				if (find) break;
			}
			return m_Data[id];
		}

		inline T& operator[](uint32 i) noexcept { return m_Data[i-1]; }

		inline void lock() const noexcept { m_Mutex.lock(); }
		inline void unlock() const noexcept { m_Mutex.unlock(); }

		inline void clear() noexcept {
			m_Data.clear();
			m_RemovedList.clear();
		}

	};

}
#pragma once

#include "vector.h"
// temp
#include "Debug.h"

namespace jsh {

	template<typename T>
	class batch_pool {
		size_t m_Capacity;

		struct Pool {
			mutable T* ptr = nullptr;
			size_t size = 0u;

			Pool(size_t size) : ptr(new T[size]) {
				jshLogln("All right borororroro");
			}
			~Pool()
			{
				if(ptr != nullptr) delete ptr;
			}

			Pool& operator= (const Pool& other) {
				ptr = other.ptr;
				size = other.size;
				other.ptr = nullptr;
				return *this;
			}
			Pool& operator= (Pool&& other) {
				ptr = other.ptr;
				size = other.size;
				other.ptr = nullptr;
				other.size = 0u;
				return *this;
			}
			Pool(const Pool& other) {
				ptr = other.ptr;
				size = other.size;
				other.ptr = nullptr;
			}
			Pool(Pool&& other) {
				ptr = other.ptr;
				size = other.size;
				other.ptr = nullptr;
				other.size = 0u;
			}
		};

		std::vector<Pool> m_Pools;
		uint32 m_Pool = 0u;

		void clear_all()
		{
			m_Pools.clear();
		}

	public:
		batch_pool(size_t capacity) : m_Capacity(capacity) 
		{
			m_Pools.emplace_back(m_Capacity);
		}
		~batch_pool()
		{
			clear_all();
		}

		inline void push(const T& t)
		{
			if (m_Pools[m_Pool].size >= m_Capacity) {
				next_pool();
			}
			Pool& pool = m_Pools[m_Pool];
			pool.ptr[pool.size++] = t;
		}
		inline void next_pool() noexcept
		{
			m_Pool++;
			if (m_Pools.size() <= m_Pool) m_Pools.emplace_back(m_Capacity);
		}

		inline void reset(size_t capacity = 0u) noexcept
		{
			// reallocate data
			if (capacity != 0 && capacity != m_Capacity) {
				m_Capacity = capacity;
				clear_all();
				m_Pools.emplace_back(m_Capacity);
				m_Pool = 0u;
			}
			// reset Pools
			for (uint32 i = 0; i <= m_Pool; ++i) {
				m_Pools[i].size = 0u;
			}
			m_Pool = 0u;
		}
		inline void clear() noexcept
		{
			m_Pools.resize(1);
			m_Pools[0].size = 0u;
		}
		
		// getters
		inline size_t size() const noexcept { return m_Capacity; }
		inline uint32 pool_count() const noexcept { return m_Pool + ((m_Pools[m_Pool].size == 0) ? 0u : 1u); }
		inline T* get_pool(uint32 i) noexcept { return m_Pools[i].ptr; }
		inline size_t get_pool_size(uint32 i) noexcept { return m_Pools[i].size; }

	};

}
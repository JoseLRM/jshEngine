#pragma once

#include "..//..//common.h"

namespace jsh {

	template<typename T>
	class vector {
		T* m_Data = nullptr;

		size_t m_Size = 0;
		size_t m_Pos  = 0;

	private:
		inline void internal_alloc(size_t newSize) {
			T* newData = new T[newSize];
			memcpy(newData, m_Data, m_Pos * sizeof(T));

			delete m_Data;
			m_Data = newData;
			m_Size = newSize;
		}

	public:
		vector() {}
		vector(T* ptr, size_t size) : m_Data(ptr), m_Size(size), m_Pos(size) {}

		void operator=(const vector<T>& other) {
			if (m_Data) delete m_Data;
			m_Data = new T[other.m_Size];
			m_Size = other.m_Size;
			m_Pos = other.m_Pos;
			memcpy(m_Data, other.m_Data, m_Pos * sizeof(T));
		}

		vector(const vector<T>& other)
		{
			*this = other;
		}

		~vector() {
			if (m_Data) delete m_Data;
		}

		// allocations
		void reserve(size_t cant) noexcept
		{
			size_t nextSize = m_Pos + cant;
			if (nextSize > m_Size) {
				internal_alloc(nextSize);
			}
		}

		void resize(size_t size)
		{
			if (size > m_Size) {
				internal_alloc(size);
			}
			else if (size < m_Size) {
				T* newData = new T[size];
				memcpy(newData, m_Data, size * sizeof(T));
				delete m_Data;
				newData = m_Data;
				m_Size = size;
				if (m_Size < m_Pos) m_Pos = m_Size;
			}
		}

		void clear()
		{
			m_Pos = 0;
		}

		// setters
		inline T& operator[](size_t i) noexcept { return m_Data[i]; }

		void push_back(const T& t) {
			reserve(1);
			m_Data[m_Pos++] = t;
		}

		void swap(size_t i0, size_t i1) noexcept
		{
			const T aux = this->operator[](i0);
			memcpy(&m_Data[i0], &m_Data[i1], sizeof(T));
			memcpy(&m_Data[i1], &aux, sizeof(T));
		}

		void insert(size_t i, const T& t) noexcept
		{
			if (m_Pos == m_Size) reserve(5);
			memcpy(&m_Data[i + 1], &m_Data[i], (m_Pos - i) * sizeof(T));
			m_Data[i] = t;
			m_Pos++;
		}

		// getters
		inline size_t size() const noexcept { return m_Pos; }
		inline bool empty() const noexcept { return m_Pos == 0; }
		inline size_t capacity() const noexcept { return m_Size; }

		inline T& front() noexcept
		{
			return m_Data[0];
		}

		inline T& back() noexcept
		{
			return m_Data[m_Pos - 1];
		}

		inline void pop_back() noexcept
		{
			m_Pos--;
		}

	};

}
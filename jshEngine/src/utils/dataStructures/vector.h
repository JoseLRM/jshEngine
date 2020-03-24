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
			T* newData = reinterpret_cast<T*>(malloc(newSize * sizeof(T)));
			if (newData == 0) return;
			if (newSize < m_Pos) m_Pos = newSize;
			memcpy(newData, m_Data, m_Pos * sizeof(T));
			free(m_Data);
			m_Data = newData;
			m_Size = newSize;
		}

		inline void free_data()
		{
			if (m_Data) {
				free(m_Data);
				m_Data = nullptr;
			}
		}

	public:
		vector() : m_Data(nullptr) {}
		vector(void* ptr, size_t size) : m_Data((T*)ptr), m_Size(size), m_Pos(size) {}

		vector(const vector<T>& other)
		{
			free_data();
			if (other.m_Data && other.m_Pos != 0) {
				internal_alloc(other.m_Pos);
				memcpy(m_Data, other.m_Data, other.m_Pos * sizeof(T));
				m_Pos = other.m_Pos;
				m_Size = m_Pos;
			}
		}

		~vector() {
			free_data();
		}

		// allocations
		void reserve(size_t cant) noexcept
		{
			internal_alloc(m_Pos + cant);
		}

		void resize(size_t size)
		{
			if (size != m_Size) {
				internal_alloc(size);
			}
		}

		void clear(bool del = true)
		{
			m_Pos = 0u;
			if (del) {
				free_data();
				m_Size = 0u;
			}
		}

		inline void add_pos(size_t p)
		{
			m_Pos += p;
		}
		inline void sub_pos(size_t p)
		{
			m_Pos -= p;
		}

		inline bool reserve_request() const noexcept { return m_Pos == m_Size; }

		// setters
		inline T& operator[](size_t i) noexcept { return m_Data[i]; }

		inline void push_back(const T& t, size_t reserveRate = 1u) {
			if(reserve_request()) reserve(reserveRate);
			push_back_nr(t);
		}

		inline void push_back_nr(const T& t)
		{
			memcpy(&m_Data[m_Pos++], &t, sizeof(T));
		}

		void swap(size_t i0, size_t i1, size_t size) noexcept
		{
			// auxiliar
			byte* aux = new byte[size * sizeof(T)];
			memcpy(aux, &m_Data[i0], size * sizeof(T));

			memcpy(&m_Data[i0], &m_Data[i1], size * sizeof(T));
			memcpy(&m_Data[i1], &aux, size * sizeof(T));
		}

		void swap(size_t i0, size_t i1) noexcept
		{
			const T aux = this->operator[](i0);
			memcpy(&m_Data[i0], &m_Data[i1], sizeof(T));
			memcpy(&m_Data[i1], &aux, sizeof(T));
		}

		void insert(size_t i, const T& t) noexcept
		{
			size_t moveSize = (m_Pos - i) * sizeof(T);
			T* aux = reinterpret_cast<T*>(malloc(moveSize));
			if (aux == 0) return;
			memcpy(aux, &m_Data[i], moveSize);
			if (m_Pos == m_Size) reserve(1);
			memcpy(&m_Data[i + 1], aux, moveSize);
			free(aux);
			memcpy(&m_Data[i], &t, sizeof(T));
			m_Pos++;
		}

		// getters
		inline size_t size() const noexcept { return m_Pos; }
		inline bool empty() const noexcept { return m_Pos == 0; }
		inline size_t capacity() const noexcept { return m_Size; }
		inline T* data() const noexcept { return m_Data; }

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
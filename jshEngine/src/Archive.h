#pragma once

#include "common.h"

namespace jsh {

	class Archive {
		uint8* m_Data = nullptr;

		size_t m_Capacity = 0u;
		size_t m_Size = 0u;

		size_t m_ReadPos = 0u;

	public:
		Archive() = default;
		Archive(const Archive& other);
		Archive(Archive&& other) noexcept;
		~Archive();

		Archive& operator=(const Archive& other) noexcept;
		Archive& operator=(Archive&& other) noexcept;

		bool LoadFile(const char* filePath);
		bool SaveFile(const char* filePath);

		void Write(void* data, size_t size) noexcept;
		void Read(void* data, size_t size) noexcept;

		inline bool CanRead() const noexcept { return m_ReadPos != m_Size; }

		inline void BeginRead() noexcept { m_ReadPos = 0u; }

		// write functions
		template<typename T>
		inline Archive& operator<<(const T& n) noexcept
		{
			Write(const_cast<T*>(&n), sizeof(T));
			return *this;
		}

		template<typename T>
		inline Archive& operator<<(const std::vector<T>& v) noexcept
		{
			this->operator<<(v.size());
			Write(const_cast<T*>(v.data()), v.size() * sizeof(T));
			return *this;
		}

		inline Archive& operator<<(const char* c) noexcept
		{
			size_t size = std::strlen(c);
			this->operator<<(size);
			Write(const_cast<char*>(c), size);
			return *this;
		}
		inline Archive& operator<<(const std::string& str) noexcept
		{
			this->operator<<(str.c_str());
			return *this;
		}
		inline Archive& operator<<(const wchar* c) noexcept
		{
			size_t size = std::wcslen(c);
			this->operator<<(size);
			Write(const_cast<wchar*>(c), size);
			return *this;
		}
		inline Archive& operator<<(const std::wstring& str) noexcept
		{
			this->operator<<(str.c_str());
			return *this;
		}

		// read functions
		template<typename T>
		inline Archive& operator>>(T& n)
		{
			Read(&n, sizeof(T));
			return *this;
		}

		template<typename T>
		inline Archive& operator>>(std::vector<T>& v) noexcept
		{
			size_t size;
			size_t lastSize = v.size();
			this->operator>>(size);
			v.resize(lastSize + size);
			Read(v.data() + lastSize, size * sizeof(T));
			return *this;
		}
		inline Archive& operator>>(std::string& str) noexcept
		{
			size_t size;
			size_t lastSize = str.size();
			this->operator>>(size);
			str.resize(lastSize + size);
			Read(&str[lastSize], size);
			return *this;
		}
		inline Archive& operator>>(std::wstring& str) noexcept
		{
			size_t size;
			size_t lastSize = str.size();
			this->operator>>(size);
			str.resize(lastSize + size);
			Read(&str[lastSize], size);
			return *this;
		}

		void Reserve(size_t size);
		void Clear();
		
	private:
		void Allocate(size_t size);

	};

	class TxtFile : public std::stringstream {
	public:
		bool LoadFile(const char* filePath);
		bool SaveFile(const char* filePath, bool clear = true);
	};

}
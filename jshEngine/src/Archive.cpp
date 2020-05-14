#include "common.h"
#include "Archive.h"

#include <fstream>

using namespace std;

namespace jsh {

	// COPY
	Archive::Archive(const Archive& other)
	{
		this->operator=(other);
	}
	Archive& Archive::operator=(const Archive& other) noexcept
	{
		Clear();
		m_Data = new uint8[other.m_Size];
		memcpy(m_Data, other.m_Data, other.m_Size);
		m_Capacity = other.m_Size;
		m_Size = other.m_Size;
		m_ReadPos = 0u;
		return *this;
	}

	// MOVE
	Archive::Archive(Archive&& other) noexcept
	{
		this->operator=(std::move(other));
	}
	Archive& Archive::operator=(Archive&& other) noexcept
	{
		Clear();
		m_Size = other.m_Size;
		m_Capacity = other.m_Capacity;
		m_Data = other.m_Data;
		m_ReadPos = other.m_ReadPos;
		other.m_Size = 0u;
		other.m_Capacity = 0u;
		other.m_Data = nullptr;
		other.m_ReadPos = 0u;
		return *this;
	}

	Archive::~Archive()
	{
		Clear();
	}

	bool Archive::LoadFile(const char* filePath)
	{
		Clear();
		std::ifstream s(filePath, ios::binary | ios::ate);
		if (s.is_open()) {
			size_t size = s.tellg();
			s.seekg(0u);

			Allocate(size);
			m_Size = size;
			s.read(reinterpret_cast<char*>(m_Data), size);
			s.close();
			return true;
		}
		else {
			jshDebug::LogE("File '%s' not found", filePath);
			return false;
		}
	}

	bool Archive::SaveFile(const char* filePath)
	{
		ofstream s(filePath, ios::binary | ios::trunc);
		if (s.is_open()) {
			s.write(reinterpret_cast<const char*>(m_Data), m_Size);
			s.close();
			return true;
		}
		else {
			jshDebug::LogE("File '%s' not found", filePath);
			return false;
		}
	}

	void Archive::Write(void* data, size_t size) noexcept
	{
		size_t nextSize = m_Size + size;
		if (nextSize > m_Capacity) Allocate(nextSize);

		memcpy(m_Data + m_Size, data, size);
		m_Size = nextSize;
	}
	void Archive::Read(void* data, size_t size) noexcept
	{
		JSH_ASSERT(m_Size >= m_ReadPos + size);
		memcpy(data, m_Data + m_ReadPos, size);
		m_ReadPos += size;
	}

	void Archive::Reserve(size_t size)
	{
		Allocate(m_Capacity + size);
	}
	void Archive::Clear()
	{
		if (m_Data != nullptr) {
			delete[] m_Data;
			m_Data = nullptr;
		}
		m_Capacity = 0u;
		m_Size = 0u;
		m_ReadPos = 0u;
	}
	void Archive::Allocate(size_t size)
	{
		size_t newSize = m_Size < size ? m_Size : size;
		uint8* newData = new uint8[size];
		if (m_Data != nullptr) {
			memcpy(newData, m_Data, newSize);
			delete[] m_Data;
		}		
		m_Size = newSize;
		m_Capacity = size;
		m_Data = newData;
	}

	//////////////////////////////////////////////

	bool TxtFile::LoadFile(const char* filePath)
	{
		ifstream s(filePath, ios::binary || ios::ate);
		if (s.is_open()) {

			size_t size = s.tellg();
			s.seekg(0u);

			this->str().resize(size);
			s.read(&this->str()[0], size);

			s.close();
			return true;
		}
		else {
			jshDebug::Log("Txt File not found '%s'", filePath);
			return false;
		}
	}
	bool TxtFile::SaveFile(const char* filePath, bool clear)
	{
		ofstream s;
		s.open(filePath, ios::binary | ios::ate | (clear ? ios::trunc : ios::app));
		if (s.is_open()) {
			s.write(this->str().c_str(), this->str().size());
			s.close();
			return true;
		}
		else {
			jshDebug::Log("Txt File not found '%s'", filePath);
			return false;
		}
	}

}
#include "ConstantBuffer.h"

namespace jsh {

	void ConstantBuffer::Create(uint32 CPUAccessFlags, JSH_USAGE usage, uint32 miscFlags)
	{
		if (m_Created) return;

		// align data
		uint32 reserve = 16 - (m_Data.size() % 16);
		if (reserve != 0) {
			uint32 index = m_Data.size();
			m_Data.resize(index + reserve);
			jshZeroMemory(m_Data.data() + index, reserve);
		}

		JSH_BUFFER_DESC desc;
		desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = m_Data.size();
		desc.CPUAccessFlags = CPUAccessFlags;
		desc.MiscFlags = miscFlags;
		desc.StructureByteStride = 0u;
		desc.Usage = usage;
		JSH_SUBRESOURCE_DATA sdata;
		sdata.pSysMem = m_Data.data();
		jshGraphics::CreateBuffer(&desc, &sdata, &m_Buffer);

		m_Created = true;
	}

	void ConstantBuffer::Reset() noexcept
	{
		m_Created = false;
		m_Layout.clear();
		m_Data.clear();
	}

	void ConstantBuffer::AddValue(const char* name, void* value, uint32 size)
	{
		assert(!m_Created);
		uint32 index = m_Data.size();
		if ((index + size - 1) / 16 != index / 16) index += index % 16;
		m_Layout[name] = Value(size, index);
		m_Data.resize(index + size);
		memcpy(m_Data.data() + index, value, size);
		m_Size = m_Data.size();
	}

	void ConstantBuffer::SetValue(const char* name, void* value)
	{
		m_Modified = true;
		Value& v = m_Layout[name];
		memcpy(&m_Data[v.offset], value, v.size);
	}

	void* ConstantBuffer::GetValue(const char* name)
	{
		return &m_Data[m_Layout[name].offset];
	}

}
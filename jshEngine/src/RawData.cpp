#include "RawData.h"

#include "Debug.h"

namespace jsh {

	void RawData::Create()
	{
		// VERTEX BUFFER
		{
			// calculate buffer size, vertex size and vertex layout offset

			size_t bufferSize = 0u;
			size_t vertexSize = 0u;
			for (uint32 i = 0; i < m_VertexLayout.size(); ++i) {
				m_VertexLayout[i].offset = vertexSize;
				vertexSize += jshGraphics::GetFormatStride(m_VertexLayout[i].prop.format);
			}
			bufferSize = vertexSize * size_t(m_VertexCount);

			// allocate vertex data
			byte* data = new byte[bufferSize];
			
			// set data
			for (uint32 i = 0; i < m_VertexLayout.size(); ++i) {
				VertexLayout& layout = m_VertexLayout[i];
				uint32 offset = 0u;
				uint32 stride = jshGraphics::GetFormatStride(layout.prop.format);
				for (size_t j = layout.offset; j < bufferSize; j += vertexSize) {
					memcpy(data + j, layout.data + offset, stride);
					offset += stride;
				}
			}

			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_VERTEX_BUFFER;
			desc.ByteWidth = bufferSize;
			desc.CPUAccessFlags = 0u;
			desc.MiscFlags = 0u;
			desc.StructureByteStride = vertexSize;
			desc.Usage = JSH_USAGE_IMMUTABLE;
			JSH_SUBRESOURCE_DATA sdata;
			sdata.pSysMem = data;
			jshGraphics::CreateBuffer(&desc, &sdata, &m_VertexBuffer);

			delete[] data;
		}

		// INDEX BUFFER
		if(m_IndexCount > 0 && m_pIndices != nullptr)
		{
			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_INDEX_BUFFER;
			desc.ByteWidth = m_IndexCount * sizeof(uint32);
			desc.CPUAccessFlags = 0u;
			desc.MiscFlags = 0u;
			desc.StructureByteStride = sizeof(uint32);
			desc.Usage = JSH_USAGE_IMMUTABLE;
			JSH_SUBRESOURCE_DATA sdata;
			sdata.pSysMem = m_pIndices;
			jshGraphics::CreateBuffer(&desc, &sdata, &m_IndexBuffer);
		}
	}

	void RawData::Draw(uint32 instances, jsh::CommandList cmd)
	{
		assert(instances != 0);

		if (m_IndexBuffer.IsValid()) {

			if (instances == 1) jshGraphics::DrawIndexed(m_IndexCount, cmd);
			else jshGraphics::DrawIndexedInstanced(m_IndexCount, instances, 0u, 0u, 0u, cmd);

		}
		else {

			if (instances == 1) jshGraphics::Draw(m_VertexCount, cmd);
			else jshGraphics::DrawInstanced(4, instances, 0u, 0u, cmd);

		}
	}

}
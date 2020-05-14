#pragma once

#include "GraphicsPrimitives.h"

namespace jsh {

	class Mesh;

	class RawData {

		Buffer m_VertexBuffer;
		Buffer m_IndexBuffer;
		JSH_TOPOLOGY m_Topology = JSH_TOPOLOGY_TRIANGLES;

		struct VertexLayout {
			VertexProperty prop;
			uint32 offset;
			uint8* data;
			VertexLayout(const VertexProperty& vp, uint32 offset, uint8* data) : prop(vp), offset(offset), data(data) {}
		};
		std::vector<VertexLayout> m_VertexLayout;
		uint32* m_pIndices;

		uint32 m_VertexCount = 0u;
		uint32 m_IndexCount = 0u;
		JSH_FORMAT m_IndexBufferFormat;

#ifdef JSH_ENGINE
	public:
#endif
		RawData() = default;

	public:

		void Create();
		void Draw(uint32 instances, CommandList cmd);

		// setters
		inline void AddVertexLayout(const char* name, JSH_FORMAT format, uint32 index, void* data) noexcept
		{
			m_VertexLayout.emplace_back(VertexProperty(name, format, index), 0u, reinterpret_cast<uint8*>(data));
		}
		inline void SetIndices(uint32* data, uint32 count, JSH_FORMAT format) noexcept
		{
			m_pIndices = data;
			m_IndexCount = count;
			m_IndexBufferFormat = format;
		}

		inline void SetVertexCount(uint32 count) noexcept 
		{ 
			m_VertexCount = count; 
		}

		inline void SetTopology(JSH_TOPOLOGY topology) noexcept { m_Topology = topology; }
		
		// getters
		inline uint32 GetVertexCount() const noexcept { return m_VertexCount; }
		inline uint32 GetIndexCount() const noexcept { return m_IndexCount; }
		inline JSH_TOPOLOGY GetTopology() const noexcept { return m_Topology; }

		inline std::vector<VertexLayout>& GetVertexLayout() noexcept { return m_VertexLayout; }

		inline Buffer GetVertexBuffer() const noexcept { return m_VertexBuffer; }
		inline Buffer GetIndexBuffer() const noexcept { return m_IndexBuffer; }

		inline JSH_FORMAT GetIndexBufferFormat() const noexcept { return m_IndexBufferFormat; }

	};


}
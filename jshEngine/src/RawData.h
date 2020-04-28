#pragma once

#include "GraphicsPrimitives.h"

namespace jsh {

	typedef uint8 RawDataFlags;

#define JSH_RAW_DATA_NONE			0
#define JSH_RAW_DATA_POSITIONS		BIT(0)
#define JSH_RAW_DATA_NORMALS		BIT(1)
#define JSH_RAW_DATA_TEX_COORDS		BIT(2)
#define JSH_RAW_DATA_INDICES		BIT(3)
#define JSH_RAW_DATA_COLORS			BIT(4)
#define JSH_RAW_DATA_TANGENTS		BIT(5)
#define JSH_RAW_DATA_BITANGENTS		BIT(6)


	class RawData {
		RawDataFlags m_DataFlags;
		uint8 m_CreationType = 0u;

		float* m_pPosData = nullptr;
		float* m_pNorData = nullptr;
		float* m_pTanData = nullptr;
		float* m_pBitanData = nullptr;
		uint32 m_VertexCount = 0u;

		float* m_pTexCoordsData = nullptr;

		uint8* m_pColorData = nullptr;

		uint32* m_pIndexData = nullptr;
		uint32 m_IndexCount = 0u;

		bool m_Created = false;

		// graphics primitives
		Buffer m_VertexBuffer;
		Buffer m_IndexBuffer;

#ifdef JSH_ENGINE
	public:
#endif
		RawData();
	public:
		void Create();

		void Bind(jsh::CommandList cmd);

		// setters
		void SetPositionsAndNormals(float* pos, float* nor, uint32 vertices);
		void SetColors(uint8* col);
		void SetIndices(uint32* data, uint32 indices);
		void SetTextureCoords(float* coords);
		void SetTangents(float* tan);
		void SetBitangents(float* bitan);

		// getters
		inline bool IsCreated() const noexcept { return m_Created; }
		inline bool IsValid() const noexcept {
			return IsCreated() ? true : m_DataFlags != JSH_RAW_DATA_NONE &&
				(m_DataFlags & JSH_RAW_DATA_POSITIONS && m_DataFlags & JSH_RAW_DATA_NORMALS && m_DataFlags & JSH_RAW_DATA_INDICES);
		}
		inline RawDataFlags GetFlags() const noexcept { return m_DataFlags; }
		inline uint32 GetIndexCount() const noexcept { return m_IndexCount; }

	private:
		void CreateSolid();
		void CreateSimpleTex();
		void CreateSimpleCol();
		void CreateNormal();

	};


}
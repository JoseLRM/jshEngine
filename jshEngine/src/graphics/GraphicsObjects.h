#pragma once

#include "GraphicsPrimitives.h"
#include "..//utils/dataStructures/vector.h"

namespace jsh {
		
	// TODO: Material
	// TODO: Light

	// Shader
	struct Shader {
		VertexShader vs;
		PixelShader ps;
	};

	// Mesh
	typedef uint16 JSH_RAW_DATA;

	#define JSH_RAW_DATA_NONE			0
	#define JSH_RAW_DATA_POSITIONS		BIT(0)
	#define JSH_RAW_DATA_NORMALS		BIT(1)
	#define JSH_RAW_DATA_TEX_COORDS		BIT(2)
	#define JSH_RAW_DATA_INDICES		BIT(3)

	class RawData {
		JSH_RAW_DATA m_DataFlags;

		float* m_pPosData = nullptr;
		float* m_pNorData = nullptr;
		uint32 m_VertexCount = 0u;

		float* m_pTexCoordsData = nullptr;

		uint32* m_pIndexData = nullptr;
		uint32 m_IndexCount = 0u;

		bool m_Created = false;

		// graphics primitives
		jsh::vector<Bindable> m_Bindables;
#ifdef JSH_ENGINE
	public:
#endif
		RawData();
		void Destroy();
	public:
		// create methods
		void Create();

		void Bind();

		// setters
		void SetPositionsAndNormals(float* pos, float* nor, uint32 vertices);
		void SetIndices(uint32* data, uint32 indices);
		void SetTextureCoords(float* coords);

		// getters
		inline bool IsCreated() const noexcept { return m_Created; }
		inline bool IsValid() const noexcept {
			return IsCreated() ? true : m_DataFlags != JSH_RAW_DATA_NONE &&
				(m_DataFlags & JSH_RAW_DATA_POSITIONS && m_DataFlags & JSH_RAW_DATA_NORMALS && m_DataFlags & JSH_RAW_DATA_INDICES);
		}

		inline uint32 GetIndexCount() const noexcept { return m_IndexCount; }

	private:
		void CreateSolid();
		void CreateSimpleTex();

	};	

	class Mesh {
	public:
		RawData* rawData;
		Texture diffuseMap;

	public:
		Mesh();
		Mesh(RawData* rawData);
		Mesh(RawData* rawData, Texture diffuseMap);

		void Bind() const noexcept;

	};

	class Model {
#ifdef JSH_ENGINE
	public:
#endif
		Model();
	public:
		jsh::vector<Mesh> meshes;
	};

}

namespace jshGraphics {

	//////////////////////SHADER/////////////////////////////////
	void CreateShader(const char* name, const jsh::Shader& shader);
	const jsh::Shader& GetShader(const char* name);

	//////////////////////RAW DATA/////////////////////////////////
	jsh::RawData* CreateRawData(const char* name);
	jsh::RawData* GetRawData(const char* name);
	void DestroyRawData(const char* name);

	//////////////////////MODEL/////////////////////////////////
	jsh::Model* CreateModel(const char* name);
	jsh::Model* GetModel(const char* name);
	void DestroyModel(const char* name);

}


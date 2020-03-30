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
	typedef uint16 JSH_MESH_DATA;

	#define JSH_MESH_DATA_NONE			0
	#define JSH_MESH_DATA_POSANDNOR		BIT(0)
	#define JSH_MESH_DATA_INDICES		BIT(1)
	#define JSH_MESH_DATA_TEXTURE		BIT(2)
	#define JSH_MESH_DATA_COLOR			BIT(3)
	#define JSH_MESH_DATA_DIFFUSE_MAP	BIT(4)
	#define JSH_MESH_DATA_NORMAL_MAP	BIT(5)

	class Mesh {
		JSH_MESH_DATA m_DataFlags;

		float* m_pPosData = nullptr;
		float* m_pNorData = nullptr;
		uint32 m_VertexCount = 0u;

		uint32* m_pIndexData = nullptr;
		uint32 m_IndexCount = 0u;

		bool m_Created = false;

		// graphics primitives
		jsh::vector<Bindable> m_Bindables;

	public:
		Mesh();

		// create methods
		void Create();
		void Close();

		void Bind();

		// setters
		void SetPositionsAndNormals(float* pos, float* nor, uint32 vertices);
		void SetIndices(uint32* data, uint32 indices);

		// getters
		inline bool IsCreated() const noexcept { return m_Created; }
		inline bool IsValid() const noexcept {
			return IsCreated() ? true : m_DataFlags != JSH_MESH_DATA_NONE &&
				(m_DataFlags & JSH_MESH_DATA_POSANDNOR && m_DataFlags & JSH_MESH_DATA_INDICES);
		}

		inline uint32 GetIndexCount() const noexcept { return m_IndexCount; }

	private:
		void CreateSolid();

	};	

	class Model {
	public:
		jsh::vector<Mesh*> meshes;
	};

}

namespace jshGraphics {

	void CreateShader(const char* name, const jsh::Shader& shader);
	const jsh::Shader& GetShader(const char* name);

}


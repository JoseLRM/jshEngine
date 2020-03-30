#include "GraphicsObjects.h"

#include "..//utils/dataStructures/memory_pool.h"
#include "..//Debug.h"
#include <map>

using namespace jsh;

namespace jshGraphics {

	std::map<const char*, Shader> g_Shaders;

	void CreateShader(const char* name, const Shader& shader)
	{
		g_Shaders[name] = shader;
	}
	const Shader& GetShader(const char* name)
	{
		return g_Shaders[name];
	}

}

namespace jsh {

	jsh::Mesh::Mesh() : m_DataFlags(JSH_MESH_DATA_NONE) {}

	void jsh::Mesh::Create()
	{
		if (!IsValid()) {
			jshLogE("Can't create an empty mesh");
			return;
		}
		if (IsCreated()) {
			jshLogW("Mesh already created");
		}

		m_Created = true;

		JSH_MESH_DATA basicFlag = JSH_MESH_DATA_INDICES | JSH_MESH_DATA_POSANDNOR;
		if (m_DataFlags == basicFlag) CreateSolid();

	}
	void jsh::Mesh::Close()
	{
		// TODO: llamar jshGraphics::DestroyX() y antes de destruir
		// buscar si algun modelo esta utlizandolo
	}

	void Mesh::Bind()
	{
		assert(IsCreated());
		for (uint32 i = 0; i < m_Bindables.size(); ++i) {
			jshGraphics::Bind(m_Bindables[i]);
		}
	}

	void jsh::Mesh::SetPositionsAndNormals(float* pos, float* nor, uint32 vertices)
	{
		m_pPosData = pos;
		m_pNorData = nor;
		m_VertexCount = vertices;

		if (pos && nor) m_DataFlags |= JSH_MESH_DATA_POSANDNOR;
	}

	void jsh::Mesh::SetIndices(uint32* data, uint32 indices)
	{
		m_pIndexData = data;
		m_IndexCount = indices;
		if (data) m_DataFlags |= JSH_MESH_DATA_INDICES;
	}

	void jsh::Mesh::CreateSolid()
	{
		float* vData = new float[m_VertexCount * 6u];
		size_t bufferPtr = 0u;
		for (uint32 i = 0; i < m_VertexCount * 3; i += 3) {
			vData[bufferPtr++] = m_pPosData[i + 0];
			vData[bufferPtr++] = m_pPosData[i + 1];
			vData[bufferPtr++] = m_pPosData[i + 2];
			vData[bufferPtr++] = m_pNorData[i + 0];
			vData[bufferPtr++] = m_pNorData[i + 1];
			vData[bufferPtr++] = m_pNorData[i + 2];
		}

		Buffer vertexBuffer = jshGraphics::CreateBuffer(vData, m_VertexCount * 6 * sizeof(float), 6 * sizeof(float), JSH_USAGE_IMMUTABLE, 0u, JSH_BUFFER_TYPE_VERTEX);
		Buffer indexBuffer = jshGraphics::CreateBuffer(m_pIndexData, m_IndexCount * sizeof(uint32), sizeof(uint32), JSH_USAGE_IMMUTABLE, 0u, JSH_BUFFER_TYPE_INDEX);
		assert(vertexBuffer != INVALID_BUFFER);
		assert(indexBuffer != INVALID_BUFFER);

		const Shader& shader = jshGraphics::GetShader("SolidShader");

		const JSH_INPUT_ELEMENT_DESC desc[] = {
				{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u},
				{"Normal", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 3 * sizeof(float), 0u}
		};

		InputLayout inputLayout = jshGraphics::CreateInputLayout(desc, 2, shader.vs);

		m_Bindables.reserve(5u);
		m_Bindables.push_back_nr({ vertexBuffer, JSH_GRAPHICS_PRIMITIVE_VERTEX_BUFFER });
		m_Bindables.push_back_nr({ indexBuffer, JSH_GRAPHICS_PRIMITIVE_INDEX_BUFFER });
		m_Bindables.push_back_nr({ shader.vs, JSH_GRAPHICS_PRIMITIVE_VERTEX_SHADER });
		m_Bindables.push_back_nr({ shader.ps, JSH_GRAPHICS_PRIMITIVE_PIXEL_SHADER });
		m_Bindables.push_back_nr({ inputLayout, JSH_GRAPHICS_PRIMITIVE_INPUT_LAYOUT });

		delete[] vData;
	}
}

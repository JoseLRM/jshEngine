#include "GraphicsObjects.h"

#include "..//utils/dataStructures/memory_pool.h"
#include "..//Debug.h"
#include <unordered_map>

using namespace jsh;

namespace jshGraphics {

	//////////////////////SHADER/////////////////////////////////
	std::unordered_map<std::string, Shader> g_Shaders;

	void CreateShader(const char* name, const Shader& shader)
	{
		g_Shaders[name] = shader;
	}
	const Shader& GetShader(const char* name)
	{
		return g_Shaders[name];
	}
	//////////////////////RAW DATA/////////////////////////////////
	std::unordered_map<std::string, RawData> g_RawData;

	RawData* CreateRawData(const char* name)
	{
		g_RawData[name] = RawData();
		return &g_RawData[name];
	}
	RawData* GetRawData(const char* name)
	{
		if (g_RawData.find(name) == g_RawData.end()) return nullptr;
		return &g_RawData[name];
	}
	void DestroyRawData(const char* name)
	{
		if (g_RawData.find(name) == g_RawData.end()) return;
		RawData& rawData = g_RawData[name];
		rawData.Destroy();
		g_RawData.erase(name);
	}

	//////////////////////MODEL/////////////////////////////////
	std::unordered_map<const char*, Model> g_Models;

	Model* CreateModel(const char* name)
	{
		g_Models[name] = Model();
		return &g_Models[name];
	}
	Model* GetModel(const char* name)
	{
		if (g_Models.find(name) == g_Models.end()) return nullptr;
		return &g_Models[name];
	}
	void DestroyModel(const char* name)
	{
		if (g_Models.find(name) == g_Models.end()) return;
		Model& model = g_Models[name];
		model.meshes.clear();
		g_Models.erase(name);
	}

}

namespace jsh {

	//////////////////////RAW DATA/////////////////////////////////
	RawData::RawData() : m_DataFlags(JSH_RAW_DATA_NONE) {}

	void RawData::Create()
	{
		if (!IsValid()) {
			jshLogE("Can't create an empty mesh");
			return;
		}
		if (IsCreated()) {
			jshLogW("Mesh already created");
		}

		m_Created = true;

		if (m_DataFlags & JSH_RAW_DATA_TEX_COORDS) {
			CreateSimpleTex();
			return;
		}

		CreateSolid();
	}
	void RawData::Destroy()
	{
		// TODO: llamar jshGraphics::DestroyX() y antes de destruir
		// buscar si algun modelo esta utlizandolo
	}

	void RawData::Bind()
	{
		assert(IsCreated());
		for (uint32 i = 0; i < m_Bindables.size(); ++i) {
			jshGraphics::Bind(m_Bindables[i]);
		}
	}

	void RawData::SetPositionsAndNormals(float* pos, float* nor, uint32 vertices)
	{
		m_pPosData = pos;
		m_pNorData = nor;
		m_VertexCount = vertices;

		if (pos) m_DataFlags |= JSH_RAW_DATA_POSITIONS;
		if (nor) m_DataFlags |= JSH_RAW_DATA_NORMALS;
	}

	void RawData::SetIndices(uint32* data, uint32 indices)
	{
		m_pIndexData = data;
		m_IndexCount = indices;
		if (data) m_DataFlags |= JSH_RAW_DATA_INDICES;
	}

	void RawData::SetTextureCoords(float* coords)
	{
		m_pTexCoordsData = coords;
		if (coords) m_DataFlags |= JSH_RAW_DATA_TEX_COORDS;
	}

	void RawData::CreateSolid()
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

		Buffer vertexBuffer = jshGraphics::CreateBuffer(vData, m_VertexCount * 6 * sizeof(float), 6 * sizeof(float), JSH_USAGE_IMMUTABLE, JSH_BUFFER_TYPE_VERTEX);
		Buffer indexBuffer = jshGraphics::CreateBuffer(m_pIndexData, m_IndexCount * sizeof(uint32), sizeof(uint32), JSH_USAGE_IMMUTABLE, JSH_BUFFER_TYPE_INDEX);
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

	void RawData::CreateSimpleTex()
	{
		float* vData = new float[m_VertexCount * 8u];
		size_t bufferPtr = 0u;
		for (uint32 i = 0; i < m_VertexCount; i++) {
			vData[bufferPtr++] = m_pPosData[i * 3u + 0];
			vData[bufferPtr++] = m_pPosData[i * 3u + 1];
			vData[bufferPtr++] = m_pPosData[i * 3u + 2];
			vData[bufferPtr++] = m_pNorData[i * 3u + 0];
			vData[bufferPtr++] = m_pNorData[i * 3u + 1];
			vData[bufferPtr++] = m_pNorData[i * 3u + 2];
			vData[bufferPtr++] = m_pTexCoordsData[i * 2u + 0];
			vData[bufferPtr++] = m_pTexCoordsData[i * 2u + 1];
		}

		Buffer vertexBuffer = jshGraphics::CreateBuffer(vData, m_VertexCount * 8u * sizeof(float), 8 * sizeof(float), JSH_USAGE_IMMUTABLE, JSH_BUFFER_TYPE_VERTEX);
		Buffer indexBuffer = jshGraphics::CreateBuffer(m_pIndexData, m_IndexCount * sizeof(uint32), sizeof(uint32), JSH_USAGE_IMMUTABLE, JSH_BUFFER_TYPE_INDEX);
		assert(vertexBuffer != INVALID_BUFFER);
		assert(indexBuffer != INVALID_BUFFER);

		const Shader& shader = jshGraphics::GetShader("SimpleTexShader");

		const JSH_INPUT_ELEMENT_DESC desc[] = {
				{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u},
				{"Normal", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 3 * sizeof(float), 0u},
				{"TexCoord", 0, JSH_FORMAT_R32G32_FLOAT, 0, true, 6 * sizeof(float), 0u}
		};

		InputLayout inputLayout = jshGraphics::CreateInputLayout(desc, 3, shader.vs);

		m_Bindables.reserve(6u);
		m_Bindables.push_back_nr({ vertexBuffer, 0u, 0u, JSH_GRAPHICS_PRIMITIVE_VERTEX_BUFFER });
		m_Bindables.push_back_nr({ indexBuffer, 0u, 0u, JSH_GRAPHICS_PRIMITIVE_INDEX_BUFFER });
		m_Bindables.push_back_nr({ shader.vs, 0u, 0u, JSH_GRAPHICS_PRIMITIVE_VERTEX_SHADER });
		m_Bindables.push_back_nr({ shader.ps, 0u, 0u, JSH_GRAPHICS_PRIMITIVE_PIXEL_SHADER });
		m_Bindables.push_back_nr({ inputLayout, 0u, 0u, JSH_GRAPHICS_PRIMITIVE_INPUT_LAYOUT });

		delete[] vData;
	}

	//////////////////////MESH/////////////////////////////////
	Mesh::Mesh() : rawData(nullptr), diffuseMap(INVALID_TEXTURE) {}
	Mesh::Mesh(RawData* rawData) : rawData(rawData), diffuseMap(INVALID_TEXTURE) {}
	Mesh::Mesh(RawData* rawData, Texture diffuseMap) : rawData(rawData), diffuseMap(diffuseMap) {}

	void Mesh::Bind() const noexcept
	{
		if (rawData) rawData->Bind();
		else return;
		if (diffuseMap != INVALID_TEXTURE) jshGraphics::BindTexture(diffuseMap, 0u, JSH_SHADER_TYPE_PIXEL);
		else {
			jshGraphics::BindTexture(0u, 0u, JSH_SHADER_TYPE_PIXEL);
		}
	}

	//////////////////////MODEL/////////////////////////////////
	Model::Model() : meshes() {}

}

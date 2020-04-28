#include "GraphicsObjects.h"

#include "Debug.h"
#include <map>
#include "Graphics.h"
#include "Renderer.h"
#include "Scene.h"

using namespace jsh;

namespace jsh {

	/////////////////////////////INSTANCE BUFFER//////////////////////////
	void InstanceBuffer::Create()
	{
		XMMATRIX aux;
		JSH_BUFFER_DESC desc;
		desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(XMMATRIX);
		desc.CPUAccessFlags = 0u;
		desc.MiscFlags = 0u;
		desc.StructureByteStride = 0u;
		desc.Usage = JSH_USAGE_DEFAULT;
		JSH_SUBRESOURCE_DATA sdata;
		sdata.pSysMem = &aux;
		jshGraphics::CreateBuffer(&desc, &sdata, &m_Buffer);
	}
	void InstanceBuffer::Bind(JSH_SHADER_TYPE shaderType, jsh::CommandList cmd) const
	{
		jshGraphics::BindConstantBuffer(m_Buffer, JSH_GFX_SLOT_CBUFFER_INSTANCE, shaderType, cmd);
	}
	void InstanceBuffer::UpdateBuffer(XMMATRIX* tm, jsh::CommandList cmd)
	{
		jshGraphics::UpdateBuffer(m_Buffer, tm, 0u, cmd);
	}

	////////////////////////////MATERIAL///////////////////////////
	void Material::Create()
	{
		JSH_BUFFER_DESC desc;
		desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(float) * 4;
		desc.CPUAccessFlags = 0u;
		desc.MiscFlags = 0u;
		desc.StructureByteStride = 0u;
		desc.Usage = JSH_USAGE_DEFAULT;
		JSH_SUBRESOURCE_DATA sres;
		sres.pSysMem = &m_SpecularIntensity;
		jshGraphics::CreateBuffer(&desc, &sres, &m_Buffer);
	}

	void Material::Bind(CommandList cmd, JSH_SHADER_TYPE shaderType)
	{
		if (m_Modified) {
			struct alignas(16) {
				float specularIntensity;
				float shininess;
			} data;

			data.specularIntensity = m_SpecularIntensity;
			data.shininess = m_Shininess;

			jshGraphics::UpdateBuffer(m_Buffer, &data, 0u, cmd);
			m_Modified = false;
		}
		jshGraphics::BindConstantBuffer(m_Buffer, JSH_GFX_SLOT_CBUFFER_MATERIAL, shaderType, cmd);
	}

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

			if (m_DataFlags & JSH_RAW_DATA_TANGENTS && m_DataFlags & JSH_RAW_DATA_BITANGENTS) {
				CreateNormal();
				m_CreationType = 4;
				return;
			}

			CreateSimpleTex();
			m_CreationType = 2;
			return;
		}

		if (m_DataFlags & JSH_RAW_DATA_COLORS) {
			CreateSimpleCol();
			m_CreationType = 3;
			return;
		}

		CreateSolid();
		m_CreationType = 1;
	}

	void RawData::Bind(jsh::CommandList cmd)
	{
		assert(IsCreated());
		
		jshGraphics::BindVertexBuffer(m_VertexBuffer, 0, cmd);
		jshGraphics::BindIndexBuffer(m_IndexBuffer, cmd);
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

	void RawData::SetColors(uint8* col)
	{
		m_pColorData = col;
		if (col) m_DataFlags |= JSH_RAW_DATA_COLORS;
	}

	void RawData::SetTextureCoords(float* coords)
	{
		m_pTexCoordsData = coords;
		if (coords) m_DataFlags |= JSH_RAW_DATA_TEX_COORDS;
	}

	void RawData::SetTangents(float* tan)
	{
		m_pTanData = tan;
		if (tan)m_DataFlags |= JSH_RAW_DATA_TANGENTS;
	}

	void RawData::SetBitangents(float* bitan)
	{
		m_pBitanData = bitan;
		if (bitan) m_DataFlags |= JSH_RAW_DATA_BITANGENTS;
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

		JSH_BUFFER_DESC vertexDesc;
		vertexDesc.BindFlags = JSH_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = m_VertexCount * 6 * sizeof(float);
		vertexDesc.CPUAccessFlags = 0u;
		vertexDesc.MiscFlags = 0u;
		vertexDesc.StructureByteStride = 6 * sizeof(float);
		vertexDesc.Usage = JSH_USAGE_IMMUTABLE;
		JSH_SUBRESOURCE_DATA vertexSData;
		vertexSData.pSysMem = vData;
		jshGraphics::CreateBuffer(&vertexDesc, &vertexSData, &m_VertexBuffer);

		JSH_BUFFER_DESC indexDesc;
		indexDesc.BindFlags = JSH_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = m_IndexCount * sizeof(uint32);
		indexDesc.CPUAccessFlags = 0u;
		indexDesc.MiscFlags = 0u;
		indexDesc.StructureByteStride = sizeof(uint32);
		indexDesc.Usage = JSH_USAGE_IMMUTABLE;
		JSH_SUBRESOURCE_DATA indexSData;
		indexSData.pSysMem = m_pIndexData;
		jshGraphics::CreateBuffer(&indexDesc, &indexSData, &m_IndexBuffer);

		assert(m_VertexBuffer.IsValid());
		assert(m_IndexBuffer.IsValid());

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

		JSH_BUFFER_DESC vertexDesc;
		vertexDesc.BindFlags = JSH_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = m_VertexCount * 8u * sizeof(float);
		vertexDesc.CPUAccessFlags = 0u;
		vertexDesc.MiscFlags = 0u;
		vertexDesc.StructureByteStride = 8 * sizeof(float);
		vertexDesc.Usage = JSH_USAGE_IMMUTABLE;
		JSH_SUBRESOURCE_DATA vertexSData;
		vertexSData.pSysMem = vData;
		jshGraphics::CreateBuffer(&vertexDesc, &vertexSData, &m_VertexBuffer);

		JSH_BUFFER_DESC indexDesc;
		indexDesc.BindFlags = JSH_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = m_IndexCount * sizeof(uint32);
		indexDesc.CPUAccessFlags = 0u;
		indexDesc.MiscFlags = 0u;
		indexDesc.StructureByteStride = sizeof(uint32);
		indexDesc.Usage = JSH_USAGE_IMMUTABLE;
		JSH_SUBRESOURCE_DATA indexSData;
		indexSData.pSysMem = m_pIndexData;
		jshGraphics::CreateBuffer(&indexDesc, &indexSData, &m_IndexBuffer);

		assert(m_VertexBuffer.IsValid());
		assert(m_IndexBuffer.IsValid());
		delete[] vData;
	}

	void RawData::CreateSimpleCol()
	{
		struct Vertex {
			vec3 position;
			vec3 normal;
			uint8 color[4];
		};
		Vertex* vData = new Vertex[m_VertexCount];
		for (uint32 i = 0; i < m_VertexCount; i++) {
			vData[i].position.x = m_pPosData[i * 3u + 0];
			vData[i].position.y = m_pPosData[i * 3u + 1];
			vData[i].position.z = m_pPosData[i * 3u + 2];
			vData[i].normal.x = m_pNorData[i * 3u + 0];
			vData[i].normal.y = m_pNorData[i * 3u + 1];
			vData[i].normal.z = m_pNorData[i * 3u + 2];
			vData[i].color[0] = m_pColorData[i * 4u + 0];
			vData[i].color[1] = m_pColorData[i * 4u + 1];
			vData[i].color[2] = m_pColorData[i * 4u + 2];
			vData[i].color[3] = m_pColorData[i * 4u + 3];
		}

		JSH_BUFFER_DESC vertexDesc;
		vertexDesc.BindFlags = JSH_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = m_VertexCount * sizeof(Vertex);
		vertexDesc.CPUAccessFlags = 0u;
		vertexDesc.MiscFlags = 0u;
		vertexDesc.StructureByteStride = sizeof(Vertex);
		vertexDesc.Usage = JSH_USAGE_IMMUTABLE;
		JSH_SUBRESOURCE_DATA vertexSData;
		vertexSData.pSysMem = vData;
		jshGraphics::CreateBuffer(&vertexDesc, &vertexSData, &m_VertexBuffer);

		JSH_BUFFER_DESC indexDesc;
		indexDesc.BindFlags = JSH_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = m_IndexCount * sizeof(uint32);
		indexDesc.CPUAccessFlags = 0u;
		indexDesc.MiscFlags = 0u;
		indexDesc.StructureByteStride = sizeof(uint32);
		indexDesc.Usage = JSH_USAGE_IMMUTABLE;
		JSH_SUBRESOURCE_DATA indexSData;
		indexSData.pSysMem = m_pIndexData;
		jshGraphics::CreateBuffer(&indexDesc, &indexSData, &m_IndexBuffer);

		assert(m_VertexBuffer.IsValid());
		assert(m_IndexBuffer.IsValid());

		delete[] vData;
	}

	void RawData::CreateNormal()
	{
		struct Vertex {
			vec3 position;
			vec3 normal;
			vec2 texCoord;
			vec3 tan;
			vec3 bitan;
		};
		Vertex* vData = new Vertex[m_VertexCount];
		for (uint32 i = 0; i < m_VertexCount; i++) {
			vData[i].position.x = m_pPosData[i * 3u + 0];
			vData[i].position.y = m_pPosData[i * 3u + 1];
			vData[i].position.z = m_pPosData[i * 3u + 2];
			vData[i].normal.x = m_pNorData[i * 3u + 0];
			vData[i].normal.y = m_pNorData[i * 3u + 1];
			vData[i].normal.z = m_pNorData[i * 3u + 2];
			vData[i].texCoord.x = m_pTexCoordsData[i * 2u + 0];
			vData[i].texCoord.y = m_pTexCoordsData[i * 2u + 1];
			vData[i].tan.x = m_pTanData[i * 3u + 0];
			vData[i].tan.y = m_pTanData[i * 3u + 1];
			vData[i].tan.z = m_pTanData[i * 3u + 2];
			vData[i].bitan.x = m_pBitanData[i * 3u + 0];
			vData[i].bitan.y = m_pBitanData[i * 3u + 1];
			vData[i].bitan.z = m_pBitanData[i * 3u + 2];
		}

		JSH_BUFFER_DESC vertexDesc;
		vertexDesc.BindFlags = JSH_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = m_VertexCount * sizeof(Vertex);
		vertexDesc.CPUAccessFlags = 0u;
		vertexDesc.MiscFlags = 0u;
		vertexDesc.StructureByteStride = sizeof(Vertex);
		vertexDesc.Usage = JSH_USAGE_IMMUTABLE;
		JSH_SUBRESOURCE_DATA vertexSData;
		vertexSData.pSysMem = vData;
		jshGraphics::CreateBuffer(&vertexDesc, &vertexSData, &m_VertexBuffer);

		JSH_BUFFER_DESC indexDesc;
		indexDesc.BindFlags = JSH_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = m_IndexCount * sizeof(uint32);
		indexDesc.CPUAccessFlags = 0u;
		indexDesc.MiscFlags = 0u;
		indexDesc.StructureByteStride = sizeof(uint32);
		indexDesc.Usage = JSH_USAGE_IMMUTABLE;
		JSH_SUBRESOURCE_DATA indexSData;
		indexSData.pSysMem = m_pIndexData;
		jshGraphics::CreateBuffer(&indexDesc, &indexSData, &m_IndexBuffer);

		assert(m_VertexBuffer.IsValid());
		assert(m_IndexBuffer.IsValid());

		delete[] vData;
	}

	//////////////////////MESH/////////////////////////////////

	//////////////////////MODEL/////////////////////////////////
	Model::Model() : root() {}

	void Model::CreateEntity(jsh::Entity entity) noexcept
	{
		AddNode(entity, &root);
	}
	void Model::AddNode(jsh::Entity parent, MeshNode* node) noexcept
	{
		if (node->mesh != nullptr) {
			MeshComponent* meshComp = jshScene::GetComponent<MeshComponent>(parent);
			meshComp->mesh = node->mesh;
			jshScene::GetComponent<NameComponent>(parent)->SetName(node->name);
			jshScene::GetTransform(meshComp->entity) = node->transform;
		}

		uint32 cant = node->sons.size();
		if (cant == 0) return;

		jsh::vector<Entity> entities;
		if(node->name.size() == 0) jshScene::CreateSEntities(parent, cant, &entities, jsh::MeshComponent());
		else jshScene::CreateSEntities(parent, cant, &entities, jsh::MeshComponent(), jsh::NameComponent());

		for (uint32 i = 0; i < cant; ++i) {
			AddNode(entities[i], &node->sons[i]);
		}
	}

}

namespace jshGraphics {

	std::map<std::string, std::shared_ptr<void>> g_NamedData;

	void Save(const char* name, std::shared_ptr<void> data)
	{
		std::string s = name;
		if (g_NamedData.find(s) != g_NamedData.end()) {
			jshLogE("Duplicated name '%s'", name);
		}
		
		g_NamedData[s] = data;
	}
	void* Get(const char* name)
	{
		auto it = g_NamedData.find(name);
		if (it == g_NamedData.end()) return nullptr;
		else {
			return (*it).second.get();
		}
	}

	// mesh - rawdata allocation
	// TODO: custom allocation
	std::map<std::string, Mesh*> g_Mesh;
	std::map<std::string, RawData*> g_RawData;
	std::map<std::string, Material*> g_Material;
	std::map<std::string, Shader*> g_Shader;
	std::map<std::string, Texture*> g_Texture;

#define CreateObject(name, obj, map) \
std::string n = name; \
if (map.find(n) == map.end()) { \
	map[n] = obj; \
} \
else { \
	jshLogW("Duplicated graphics object, name '%s'", name); \
} 

#define GetObject(name, map) \
auto it = map.find(name); \
if (it == map.end()) { \
	return nullptr; \
} \
else { \
	return (*it).second; \
}

#define RemoveObject(name, map) \
auto it = map.find(name); \
if (it != map.end()) { \
	delete (*it).second; \
	map.erase(name); \
}

	jsh::Mesh* CreateMesh(const char* name)
	{
		Mesh* mesh = new Mesh();
		CreateObject(name, mesh, g_Mesh);
		return mesh;
	}
	jsh::RawData* CreateRawData(const char* name)
	{
		RawData* rawData = new RawData();
		CreateObject(name, rawData, g_RawData);
		return rawData;
	}
	jsh::Material* CreateMaterial(const char* name)
	{
		Material* material = new Material();
		CreateObject(name, material, g_Material);
		return material;
	}
	jsh::Shader* CreateShader(const char* name)
	{
		Shader* shader = new Shader();
		CreateObject(name, shader, g_Shader);
		return shader;
	}
	jsh::Texture* CreateTexture(const char* name)
	{
		Texture* texture = new Texture();
		CreateObject(name, texture, g_Texture);
		return texture;
	}

	jsh::Mesh* GetMesh(const char* name)
	{
		GetObject(name, g_Mesh);
	}
	jsh::RawData* GetRawData(const char* name)
	{
GetObject(name, g_RawData);
	}
	jsh::Material* GetMaterial(const char* name)
	{
		GetObject(name, g_Material);
	}
	jsh::Shader* GetShader(const char* name)
	{
		GetObject(name, g_Shader);
	}
	jsh::Texture* GetTexture(const char* name)
	{
		GetObject(name, g_Texture);
	}

	void RemoveMesh(const char* name)
	{
		RemoveObject(name, g_Mesh);
	}
	void RemoveRawData(const char* name)
	{
		RemoveObject(name, g_RawData);
	}
	void RemoveMaterial(const char* name)
	{
		RemoveObject(name, g_Material);
	}
	void RemoveShader(const char* name)
	{
		RemoveObject(name, g_Shader);
	}
	void RemoveTexture(const char* name)
	{
		RemoveObject(name, g_Texture);
	}

	void ClearObjects()
	{
		g_NamedData.clear();
		for (auto& it : g_Mesh) {
			delete it.second;
		}
		for (auto& it : g_RawData) {
			delete it.second;
		}
		for (auto& it : g_Material) {
			delete it.second;
		}
		for (auto& it : g_Shader) {
			delete it.second;
		}
		for (auto& it : g_Texture) {
			delete it.second;
		}
		g_Mesh.clear();
		g_RawData.clear();
	}

#ifdef JSH_IMGUI

	template<typename Obj>
	bool ShowObjImGuiWindow(const char* typeName, std::map<std::string, Obj*>& map, const char*& selectedObj, void(*fn)())
	{
		static char name[32];
		bool result = true;
		if (ImGui::Begin(typeName)) {

			ImGui::Columns(2);

			for (auto& it : map) {

				const char* cstr = it.first.c_str();

				bool selected = cstr == selectedObj;

				if (ImGui::Selectable(it.first.c_str(), selected)) {
					selectedObj = cstr;
					for (uint32 i = 0; i < 32; ++i) {
						if (i < it.first.size()) name[i] = it.first[i];
						else name[i] = '\0';
					}
				}
			}

			ImGui::NextColumn();

			if (selectedObj != nullptr) {
				ImGui::Text(selectedObj);
				fn();

				ImGui::InputText("Name", name, 32);

				if (ImGui::Button("Update")) {
					std::string newStr = name;
					bool update = true;
					if (newStr.size() == 0) update = false;
					for (auto& it : map) {
						if (newStr == it.first) {
							update = false;
						}
					}

					if (update) {
						Obj* obj = map[selectedObj];
						map[newStr] = obj;
						map.erase(selectedObj);
						for (auto& it : map) {
							if (it.first == newStr) {
								selectedObj = it.first.c_str();
							}
						}
					}
				}
				if (ImGui::Button("Remove")) {
					delete map[selectedObj];
					map.erase(selectedObj);
				}
			}

			ImGui::NextColumn();

			if (ImGui::Button("Close")) result = false;
		}
		ImGui::End();
		return result;
	}

	const char* g_SelectedMesh = nullptr;
	void MeshInfo()
	{
		Mesh* mesh = g_Mesh[g_SelectedMesh];

		bool diffuse = mesh->HasDiffuseMap();
		bool normal = mesh->HasNormalMap();
		bool specular = mesh->HasSpecularMap();

		bool transparent = mesh->IsTransparent();

		ImGui::Checkbox("DiffuseMapping", &diffuse);
		mesh->EnableDiffuseMap(diffuse);

		ImGui::Checkbox("NormalMapping", &normal);
		mesh->EnableNormalMap(normal);

		ImGui::Checkbox("SpecularMapping", &specular);
		mesh->EnableSpecularMap(specular);

		ImGui::Checkbox("Transparent", &transparent);
		mesh->SetTransparent(transparent);

		mesh->UpdatePrimitives();
	}
	bool ShowMeshImGuiWindow()
	{
		return ShowObjImGuiWindow<jsh::Mesh>("Meshes", g_Mesh, g_SelectedMesh, MeshInfo);
	}

	const char* g_SelectedRawData = nullptr;
	void RawDataInfo()
	{
		RawData* rawData = g_RawData[g_SelectedRawData];
		
		ImGui::Text("Index Count = %u", rawData->GetIndexCount());
	}
	bool ShowRawDataImGuiWindow()
	{
		return ShowObjImGuiWindow("RawData", g_RawData, g_SelectedRawData, RawDataInfo);
	}

	const char* g_SelectedMaterial = nullptr;
	void MaterialInfo()
	{

	}
	bool ShowMaterialImGuiWindow()
	{
		return ShowObjImGuiWindow("Materials", g_Material, g_SelectedMaterial, MaterialInfo);
	}

	const char* g_SelectedShader = nullptr;
	void ShaderInfo()
	{

	}
	bool ShowShaderImGuiWindow()
	{
		return ShowObjImGuiWindow("Shaders", g_Shader, g_SelectedShader, ShaderInfo);
	}

	const char* g_SelectedTexture = nullptr;
	void TextureInfo()
	{

	}
	bool ShowTextureImGuiWindow()
	{
		return ShowObjImGuiWindow("Textures", g_Texture, g_SelectedTexture, TextureInfo);
	}

	template<typename Obj>
	Obj* GetObjImGui(std::map<std::string, Obj*>& map)
	{
		for (auto& it : map) {
			if (ImGui::Button(it.first.c_str())) return it.second;
		}
	}

	jsh::Mesh* GetMeshImGui()
	{
		return GetObjImGui(g_Mesh);
	}
	jsh::RawData* GetRawDataImGui()
	{
		return GetObjImGui(g_RawData);
	}
	jsh::Material* GetMaterialImGui()
	{
		return GetObjImGui(g_Material);
	}
	jsh::Shader* GetShaderImGui()
	{
		return GetObjImGui(g_Shader);
	}
	jsh::Texture* GetTextureImGui()
	{
		return GetObjImGui(g_Texture);
	}

#endif

}
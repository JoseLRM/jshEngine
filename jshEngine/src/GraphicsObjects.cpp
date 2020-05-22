#include "common.h"

#include "GraphicsObjects.h"

#include "Graphics.h"
#include "Renderer.h"

using namespace jsh;

namespace jsh {

	void InstanceBuffer::Create(uint32 size)
	{
		JSH_BUFFER_DESC desc;
		desc.BindFlags = JSH_BIND_VERTEX_BUFFER;
		desc.ByteWidth = size;
		desc.CPUAccessFlags = JSH_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0u;
		desc.StructureByteStride = 64;
		desc.Usage = JSH_USAGE_DYNAMIC;
		jshGraphics::CreateBuffer(&desc, nullptr, &m_Buffer);
	}
	void InstanceBuffer::Update(void* data, uint32 size, CommandList cmd)
	{
		jshGraphics::UpdateBuffer(m_Buffer, data, size, cmd);
	}

	//////////////////////MESH SHADER//////////////////////////////
	void MeshShader::Bind(RawData* rawData, CommandList cmd)
	{
		BindInputLayout(rawData, cmd);
		jshGraphics::BindVertexShader(m_VertexShader, cmd);
		jshGraphics::BindPixelShader(m_PixelShader, cmd);
	}

	void MeshShader::BindVertexShader(RawData* rawData, CommandList cmd)
	{
		BindInputLayout(rawData, cmd);
		jshGraphics::BindVertexShader(m_VertexShader, cmd);
	}

	void MeshShader::AddVertexProperty(const char* name, JSH_FORMAT format, uint32 index)
	{
		m_VertexProps.emplace_back(name, format, index);
	}

	void MeshShader::CreateInputLayoutFromRawData(RawData* rawData, InputLayout* il)
	{
		static constexpr JSH_INPUT_ELEMENT_DESC instanceElements[] = {

			{ "TM", 0, JSH_FORMAT_R32G32B32A32_FLOAT, 1u, false, 0 * sizeof(float),	1u },
			{ "TM", 1, JSH_FORMAT_R32G32B32A32_FLOAT, 1u, false, 4 * sizeof(float), 1u },
			{ "TM", 2, JSH_FORMAT_R32G32B32A32_FLOAT, 1u, false, 8 * sizeof(float), 1u },
			{ "TM", 3, JSH_FORMAT_R32G32B32A32_FLOAT, 1u, false, 12 * sizeof(float), 1u }

		};
		constexpr uint32 instanceElementsCount = sizeof(instanceElements) / sizeof(instanceElements[0]);


		auto& sProps = m_VertexProps;
		auto& rLayout = rawData->GetVertexLayout();

		uint32 descSize = uint32(sProps.size() + instanceElementsCount);
		JSH_INPUT_ELEMENT_DESC* desc = new JSH_INPUT_ELEMENT_DESC[descSize];

		// create vertex input elements
		for (uint32 i = 0; i < sProps.size(); ++i) {
			bool find = false;
			uint32 j;
			VertexProperty& prop = sProps[i];

			for (j = 0; j < rLayout.size(); ++j) {
				if (prop == rLayout[j].prop) {
					find = true;
					break;
				}
			}
			if (!find) {
				return;
			}

			desc[i] = { prop.name, prop.index, prop.format, 0u, true, rLayout[j].offset, 0u };
		}

		// create instance input elements
		{
			uint32 beginIndex = sProps.size();
			for (uint32 i = 0; i < instanceElementsCount; ++i) {
				desc[beginIndex + i] = instanceElements[i];
			}
		}

		jshGraphics::CreateInputLayout(desc, descSize, m_VertexShader, il);

		delete[] desc;
	}

	void MeshShader::BindInputLayout(RawData* rawData, CommandList cmd)
	{
		auto it = m_InputLayouts.find(rawData);
		if (it != m_InputLayouts.end()) {
			jshGraphics::BindInputLayout((*it).second, cmd);
		}
		else {
			InputLayout il;
			CreateInputLayoutFromRawData(rawData, &il);

			if (il.IsValid()) {
				jshGraphics::BindInputLayout(il, cmd);
				m_InputLayouts[rawData];
				m_InputLayouts[rawData].Move(il);
			}
			else {
				jshDebug::LogE("Shader and rawData are not compatible");
			}
		}
	}

	//////////////////////MATERIAL//////////////////////////////
	void Material::SetShader(MeshShader* shader) {
		// Destroy last material
		if (m_pShader && m_MaterialData) {
			m_pShader->DestroyMaterialData(m_MaterialData);
		}
		m_pShader = shader;
		m_MaterialData = shader->CreateMaterialData();
	}

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

		uint32 cant = uint32(node->sons.size());
		if (cant == 0) return;

		std::vector<Entity> entities;
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
			jshDebug::LogE("Duplicated name '%s'", name);
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
	std::map<std::string, MeshShader*> g_Shader;
	std::map<std::string, Texture*> g_Texture;

#define CreateObject(name, obj, map) \
std::string n = name; \
if (map.find(n) == map.end()) { \
	map[n] = obj; \
} \
else { \
	jshDebug::LogW("Duplicated graphics object, name '%s'", name); \
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
		Material* material = g_Material[g_SelectedMaterial];

		MeshShader* shader = material->GetShader();
		if (shader) {
			void* md = material->GetMaterialData();

			shader->ShowMaterialInfo(md);
		}
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
		return nullptr;
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
	jsh::Texture* GetTextureImGui()
	{
		return GetObjImGui(g_Texture);
	}

#endif

}
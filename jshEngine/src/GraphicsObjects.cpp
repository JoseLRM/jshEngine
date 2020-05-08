#include "GraphicsObjects.h"

#include "Debug.h"
#include <map>
#include "Graphics.h"
#include "Renderer.h"
#include "Scene.h"

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

		Shader* shader = material->GetShader();
		if (shader) {
			ConstantData* cd = material->GetConstantData();

			shader->ShowConstantDataInfo(*cd);
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
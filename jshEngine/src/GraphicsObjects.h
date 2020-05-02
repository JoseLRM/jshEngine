#pragma once

#include "GraphicsPrimitives.h"
#include "vector.h"
#include "Transform.h"
#include <vector>
#include "Entity.h"

#include "Mesh.h"
#include "RawData.h"
#include "Shader.h"
#include "Material.h"

namespace jsh {

	class Texture {
	public:
		TextureRes resource;
		SamplerState samplerState;
	};

	struct Sprite {
		vec4 coords = {0.f, 0.f, 1.f, 1.f};
		Texture* texture = nullptr;
	};

	/////////////////////////////INSTANCE BUFFER//////////////////////////
	class InstanceBuffer {
		Buffer m_Buffer;

	public:
		void Create();
		void Bind(JSH_SHADER_TYPE shaderType, jsh::CommandList cmd) const;
		void UpdateBuffer(XMMATRIX* tm, jsh::CommandList cmd);
	};

	////////////////////////////MODEL//////////////////////////////
	struct MeshNode {
		Mesh* mesh;
		Transform transform;
		std::string name;
		std::vector<MeshNode> sons;
	};

	class Model {
	public:

		Model();
		void CreateEntity(jsh::Entity entity) noexcept;

		MeshNode root;

	private:
		void AddNode(jsh::Entity parent, MeshNode* node) noexcept;
	};

}

namespace jshGraphics {

	void Save(const char* name, std::shared_ptr<void> data);
	void* Get(const char* name);

	jsh::Mesh* CreateMesh(const char* name);
	jsh::Mesh* GetMesh(const char* name);
	void RemoveMesh(const char* name);

	jsh::RawData* CreateRawData(const char* name);
	jsh::RawData* GetRawData(const char* name);
	void RemoveRawData(const char* name);

	jsh::Material* CreateMaterial(const char* name);
	jsh::Material* GetMaterial(const char* name);
	void RemoveMaterial(const char* name);

	jsh::Shader* CreateShader(const char* name);
	jsh::Shader* GetShader(const char* name);
	void RemoveShader(const char* name);

	jsh::Texture* CreateTexture(const char* name);
	jsh::Texture* GetTexture(const char* name);
	void RemoveTexture(const char* name);

#if defined(JSH_ENGINE) && defined(JSH_IMGUI)
	bool ShowMeshImGuiWindow();
	bool ShowRawDataImGuiWindow();
	bool ShowMaterialImGuiWindow();
	bool ShowShaderImGuiWindow();
	bool ShowTextureImGuiWindow();
#endif

	void ClearObjects();

#ifdef JSH_IMGUI
	jsh::Mesh* GetMeshImGui();
	jsh::RawData* GetRawDataImGui();
	jsh::Material* GetMaterialImGui();
	jsh::Shader* GetShaderImGui();
	jsh::Texture* GetTextureImGui();
#endif

}


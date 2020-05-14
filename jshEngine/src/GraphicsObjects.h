#pragma once

#include "Mesh.h"
#include "RawData.h"
#include "Shader.h"
#include "Material.h"

namespace jsh {

	class InstanceBuffer {
		uint32 m_Size = 0u;
		Buffer m_Buffer;

	public:
		void Create(uint32 size);
		void Update(void* data, uint32 size, CommandList cmd);
		
		inline Buffer& GetBuffer() noexcept { return m_Buffer; }
	};

	class Texture {
	public:
		TextureRes resource;
		SamplerState samplerState;

		inline void Bind(uint32 resourceSlot, uint32 samplerSlot, JSH_SHADER_TYPE shaderType, CommandList cmd) const
		{
			jshGraphics::BindTexture(resource, resourceSlot, shaderType, cmd);
			jshGraphics::BindSamplerState(samplerState, samplerSlot, shaderType, cmd);
		}
	};

	struct Sprite {
		vec4 coords = {0.f, 0.f, 1.f, 1.f};
		Texture* texture = nullptr;
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


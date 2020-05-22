#pragma once

#include "RawData.h"

namespace jsh {

	class InstanceBuffer {
		uint32 m_Size = 0u;
		Buffer m_Buffer;

	public:
		~InstanceBuffer() { m_Buffer.Release(); }

		void Create(uint32 size);
		void Update(void* data, uint32 size, CommandList cmd);
		
		inline Buffer& GetBuffer() noexcept { return m_Buffer; }
	};

	class Texture {
	public:
		TextureRes resource;
		SamplerState* samplerState;

		inline void Bind(uint32 resourceSlot, uint32 samplerSlot, JSH_SHADER_TYPE shaderType, CommandList cmd) const
		{
			jshGraphics::BindTexture(resource, resourceSlot, shaderType, cmd);
			jshGraphics::BindSamplerState(*samplerState, samplerSlot, shaderType, cmd);
		}
	};

	struct Sprite {
		vec4 coords = {0.f, 0.f, 1.f, 1.f};
		Texture* texture = nullptr;
	};

	////////////////////////////MESH SHADER//////////////////////////////
	class MeshShader {
		std::map<RawData*, InputLayout> m_InputLayouts;
		std::vector<VertexProperty> m_VertexProps;

	protected:
		VertexShader m_VertexShader;
		PixelShader m_PixelShader;

	public:
		virtual void Create() = 0;

		void Bind(RawData* rawData, CommandList cmd);
		void BindVertexShader(RawData* rawData, CommandList cmd);

		virtual void* CreateMaterialData() = 0;
		virtual void DestroyMaterialData(void* matData) = 0;
		virtual void BindMaterialData(void* matData, CommandList cmd) = 0;

#ifdef JSH_IMGUI
		virtual void ShowMaterialInfo(void* matData) {}
#endif

	protected:
		void AddVertexProperty(const char* name, JSH_FORMAT format, uint32 index);

	private:
		void CreateInputLayoutFromRawData(RawData* rawData, InputLayout* il);
		void BindInputLayout(RawData* rawData, CommandList cmd);

	};

	////////////////////////////MATERIAL//////////////////////////////
	class Material {
		MeshShader* m_pShader = nullptr;
		void* m_MaterialData = nullptr;

	public:
		void SetShader(MeshShader* shader);

		inline void* GetMaterialData() const noexcept { return m_MaterialData; }
		inline MeshShader* GetShader() const noexcept { return m_pShader; }
	};

	////////////////////////////MESH//////////////////////////////
	class Mesh {
		RawData* m_RawData = nullptr;
		Material* m_Material = nullptr;

		bool m_Valid = false;

#ifdef JSH_ENGINE
	public:
#endif
		Mesh() = default;
	public:
		inline void Draw(uint32 instances, jsh::CommandList cmd) const { m_RawData->Draw(instances, cmd); }

		inline void SetRawData(RawData* rawData) noexcept { m_RawData = rawData; }
		inline void SetMaterial(Material* material) noexcept { m_Material = material; }

		inline RawData* GetRawData() const noexcept { return m_RawData; }
		inline Material* GetMaterial() const noexcept { return m_Material; }
		inline MeshShader* GetShader() const noexcept { return m_Material->GetShader(); }

		inline bool IsValid() const noexcept { return m_Valid; }

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

	jsh::Texture* CreateTexture(const char* name);
	jsh::Texture* GetTexture(const char* name);
	void RemoveTexture(const char* name);

#if defined(JSH_ENGINE) && defined(JSH_IMGUI)
	bool ShowMeshImGuiWindow();
	bool ShowRawDataImGuiWindow();
	bool ShowMaterialImGuiWindow();
	bool ShowTextureImGuiWindow();
#endif

	void ClearObjects();

#ifdef JSH_IMGUI
	jsh::Mesh* GetMeshImGui();
	jsh::RawData* GetRawDataImGui();
	jsh::Material* GetMaterialImGui();
	jsh::Texture* GetTextureImGui();
#endif

}


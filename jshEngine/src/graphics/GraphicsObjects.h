#pragma once

#include "GraphicsPrimitives.h"
#include "..//utils/dataStructures/vector.h"
#include "..//components/Transform.h"
#include <vector>

enum JSH_TEXTURE_TYPE : uint8 {
	JSH_TEXTURE_NONE,
	JSH_TEXTURE_DIFFUSE_MAP,
	JSH_TEXTURE_NORMAL_MAP,
	JSH_TEXTURE_SPECULAR_MAP,
	JSH_TEXTURE_ATLAS
};

namespace jsh {

	typedef uint32 Entity;

	////////////////////////////TEXTURE///////////////////////////
	struct Texture {
		Resource resource;
		JSH_TEXTURE_TYPE type;

		void Bind(JSH_SHADER_TYPE shaderType, CommandList cmd) const;
	};

	////////////////////////////STATE///////////////////////////
	struct RenderState {
		Resource vertexBuffer[JSH_GFX_VERTEX_BUFFERS_COUNT];
		Resource constantBuffer[2][JSH_GFX_CONSTANT_BUFFERS_COUNT];
		Resource indexBuffer;

		VertexShader vertexShader;
		PixelShader pixelShader;
		InputLayout inputLayout;

		Resource texture[2][JSH_GFX_TEXTURES_COUNT];
		SamplerState samplerState[2][JSH_GFX_SAMPLER_STATES_COUNT];
		DepthStencilState depthStencilState;
		BlendState blendState;
		RasterizerState rasterizerState;
		RenderTargetView renderTargetView[JSH_GFX_RENDER_TARGETS_COUNT];

		Viewport viewport[JSH_GFX_VIEWPORTS_COUNT];

		void SetDefault() noexcept;
		void Bind() const;
	};

	/////////////////////////////SHADER/////////////////////////////
	struct Shader {
		VertexShader vs;
		PixelShader ps;

		InputLayout inputLayout;
#ifndef JSH_ENGINE
	private:
#endif
		Shader() = default;
	};

	/////////////////////////////INSTANCE BUFFER//////////////////////////
	class InstanceBuffer {
		Resource m_Buffer;

	public:
		void Create();
		void Bind(JSH_SHADER_TYPE shaderType, jsh::CommandList cmd) const;
		void UpdateBuffer(XMMATRIX* tm, jsh::CommandList cmd);
	};

	/////////////////////////////MATERIAL/////////////////////////////
	class Material {
		bool m_Modified = true;
		float m_SpecularIntensity = 1.f;
		float m_Shininess = 5.f;
		Resource m_Buffer;
	public:
		void Create();
		void Bind(jsh::CommandList cmd, JSH_SHADER_TYPE shaderType);

		inline float GetSpecularIntensity() const noexcept { return m_SpecularIntensity; }
		inline float GetShininess() const noexcept { return m_Shininess; }

		inline void SetSpecularIntensity(float si) noexcept
		{
			m_SpecularIntensity = si;
			m_Modified = true;
		}
		inline void SetShininess(float s) noexcept
		{
			m_Shininess = s;
			m_Modified = true;
		}

#ifndef JSH_ENGINE
	private:
#endif
		Material() = default;
	};

	////////////////////////////RAWDATA//////////////////////////////
	typedef uint8 RawDataFlags;

	#define JSH_RAW_DATA_NONE			0
	#define JSH_RAW_DATA_POSITIONS		BIT(0)
	#define JSH_RAW_DATA_NORMALS		BIT(1)
	#define JSH_RAW_DATA_TEX_COORDS		BIT(2)
	#define JSH_RAW_DATA_INDICES		BIT(3)
	#define JSH_RAW_DATA_COLORS			BIT(4)
	#define JSH_RAW_DATA_TANGENTS		BIT(5)
	#define JSH_RAW_DATA_BITANGENTS		BIT(6)

	class RawData {
		RawDataFlags m_DataFlags;
		uint8 m_CreationType = 0u;

		float* m_pPosData = nullptr;
		float* m_pNorData = nullptr;
		float* m_pTanData = nullptr;
		float* m_pBitanData = nullptr;
		uint32 m_VertexCount = 0u;

		float* m_pTexCoordsData = nullptr;

		uint8* m_pColorData = nullptr;

		uint32* m_pIndexData = nullptr;
		uint32 m_IndexCount = 0u;

		bool m_Created = false;

		// graphics primitives
		Resource m_VertexBuffer;
		Resource m_IndexBuffer;

#ifdef JSH_ENGINE
	public:
#endif
		RawData();
	public:
		void Create();

		void Bind(jsh::CommandList cmd);

		// setters
		void SetPositionsAndNormals(float* pos, float* nor, uint32 vertices);
		void SetColors(uint8* col);
		void SetIndices(uint32* data, uint32 indices);
		void SetTextureCoords(float* coords);
		void SetTangents(float* tan);
		void SetBitangents(float* bitan);

		// getters
		inline bool IsCreated() const noexcept { return m_Created; }
		inline bool IsValid() const noexcept {
			return IsCreated() ? true : m_DataFlags != JSH_RAW_DATA_NONE &&
				(m_DataFlags & JSH_RAW_DATA_POSITIONS && m_DataFlags & JSH_RAW_DATA_NORMALS && m_DataFlags & JSH_RAW_DATA_INDICES);
		}
		inline RawDataFlags GetFlags() const noexcept { return m_DataFlags; }
		inline uint32 GetIndexCount() const noexcept { return m_IndexCount; }

	private:
		void CreateSolid();
		void CreateSimpleTex();
		void CreateSimpleCol();
		void CreateNormal();

	};	

	////////////////////////////MESH//////////////////////////////
	class Mesh {
		bool m_DiffuseMapEnabled	= false;
		bool m_NormalMapEnabled		= false;
		bool m_SpecularMapEnabled	= false;

		Texture* m_DiffuseMap	= nullptr;
		Texture* m_NormalMap	= nullptr;
		Texture* m_SpecularMap	= nullptr;

		RawData* m_RawData		= nullptr;
		Material* m_Material	= nullptr;
		Shader* m_Shader		= nullptr;

		bool m_Modified = true;

#ifdef JSH_ENGINE
	public:
#endif
		Mesh() = default;
	public:
		void Bind(jsh::CommandList cmd);

		void SetTexture(Texture* texture) noexcept;
		void EnableTexture(JSH_TEXTURE_TYPE type, bool enable) noexcept;
		bool HasTexture(JSH_TEXTURE_TYPE type);

		void SetRawData(RawData* rawData) noexcept;
		void SetMaterial(Material* material) noexcept;
		void SetShader(Shader* shader) noexcept;

		void UpdatePrimitives() noexcept;

		inline RawData* GetRawData() const noexcept { return m_RawData; }
		inline Material* GetMaterial() const noexcept { return m_Material; }
		inline Shader* GetShader() const noexcept { return m_Shader; }
		Texture* GetTexture(JSH_TEXTURE_TYPE type) const noexcept;

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
	jsh::RawData* CreateRawData(const char* name);
	jsh::Material* CreateMaterial(const char* name);
	jsh::Shader* CreateShader(const char* name);
	jsh::Texture* CreateTexture(const char* name);
	jsh::Mesh* GetMesh(const char* name);
	jsh::RawData* GetRawData(const char* name);
	jsh::Material* GetMaterial(const char* name);
	jsh::Shader* GetShader(const char* name);
	jsh::Texture* GetTexture(const char* name);
	void RemoveMesh(const char* name);
	void RemoveRawData(const char* name);
	void RemoveMaterial(const char* name);
	void RemoveShader(const char* name);
	void RemoveTexture(const char* name);

#ifdef JSH_ENGINE

#ifdef JSH_IMGUI
	bool ShowMeshImGuiWindow(jsh::Mesh* mesh = nullptr);
	bool ShowRawDataImGuiWindow(jsh::RawData* rawData = nullptr);
#endif
	void ClearObjects();
#endif
}


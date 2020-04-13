#pragma once

#include "GraphicsPrimitives.h"
#include "..//utils/dataStructures/vector.h"

namespace jsh {

	typedef uint32 Entity;

	/////////////////////////////SHADER/////////////////////////////
	struct Shader {
		VertexShader vs;
		PixelShader ps;
	};

	/////////////////////////////MATERIAL/////////////////////////////
	typedef uint8 MaterialFlags;

#define JSH_MATERIAL_NONE			0
#define JSH_MATERIAL_DIFFUSE_MAP	BIT(0)
#define JSH_MATERIAL_NORMAL_MAP		BIT(1)
#define JSH_MATERIAL_SPECULAR_MAP	BIT(2)

	class Material {
		MaterialFlags m_DataFlags = JSH_MATERIAL_NONE;

		Texture m_DiffuseMap;
		Texture m_NormalMap;
		Texture m_SpecularMap;

	public:
		float specularIntensity = 1.f;
		float shininess = 5.f;

	public:
		Material();

		void SetDiffuseMap(Texture map) noexcept;
		void SetNormalMap(Texture map) noexcept;
		void SetSpecularMap(Texture map) noexcept;

		void EnableDiffuseMap(bool enable) noexcept;
		void EnableNormalMap(bool enable) noexcept;
		void EnableSpecularMap(bool enable) noexcept;

		void Bind(jsh::CommandList cmd) const;

		inline bool HasDiffuseMap() const noexcept { return m_DataFlags & JSH_MATERIAL_DIFFUSE_MAP; }
		inline bool HasNormalMap() const noexcept { return m_DataFlags & JSH_MATERIAL_NORMAL_MAP; }
		inline bool HasSpecularMap() const noexcept { return m_DataFlags & JSH_MATERIAL_SPECULAR_MAP; }
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
		jsh::Buffer m_VertexBuffer;
		jsh::Buffer m_IndexBuffer;

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
		bool CreateInputLayout(jsh::InputLayout* il, jsh::VertexShader& vs) const noexcept;

		inline uint32 GetIndexCount() const noexcept { return m_IndexCount; }

	private:
		void CreateSolid();
		void CreateSimpleTex();
		void CreateSimpleCol();
		void CreateNormal();

	};	

	////////////////////////////MESH//////////////////////////////
	class Mesh {
		InputLayout m_InputLayout;
		VertexShader m_VShader;
		PixelShader m_PShader;

#ifdef JSH_ENGINE
	public:
#endif
		Mesh();
	public:
		RawData* rawData;
		Material material;

		void Bind(jsh::CommandList cmd);

		void UpdatePrimitives() noexcept;

	};

	////////////////////////////MODEL//////////////////////////////
	struct MeshNode {
		Mesh* mesh;
		jsh::vector<MeshNode> sons;
	};

	class Model : public MeshNode {
	public:

		Model();
		void CreateEntity(jsh::Entity entity) noexcept;

	private:
		void AddNode(jsh::Entity parent, MeshNode* node) noexcept;
	};

	////////////////////////////FRAMEBUFFER//////////////////////////////
	class FrameBuffer {
		static jsh::DepthStencilState s_NullDST;
		static jsh::DepthStencilState s_DepthDST;
		static jsh::DepthStencilState s_StencilDST;
		static jsh::DepthStencilState s_DepthStencilDST;

		jsh::Texture m_Texture;
		jsh::RenderTargetView m_RenderTarget;
		uint32 m_DepthStateID = 1;

		uint32 m_Width = 0u;
		uint32 m_Height = 0u;
		bool m_ConserveResolution = true;
		bool m_Created = false;

#ifdef JSH_ENGINE
	public:
#endif
		static void Initialize();

	public:
		FrameBuffer();
		FrameBuffer(bool depthTest, bool stencilTest);
		FrameBuffer(uint32 width, uint32 height);

		void Create(bool fromBackBuffer = false);
		void Bind(jsh::CommandList cmd) const;
		void Clear(jsh::CommandList cmd) const;

		inline void SetResolution(uint32 width, uint32 height) noexcept { 
			m_Width = width; 
			m_Height = height; 
			// TODO: change resolution after creation
		}

		void EnableDepthTest(jsh::CommandList cmd);
		void DisableDepthTest(jsh::CommandList cmd);
		void EnableStencilTest(jsh::CommandList cmd);
		void DisableStencilTest(jsh::CommandList cmd);

		inline const jsh::RenderTargetView& GetRTV() const noexcept { return m_RenderTarget; }

	private:
		void BindDepthStencilState(jsh::CommandList cmd) const;
		bool IsBounded() const;

	};

}

namespace jshGraphics {

	void Save(const char* name, std::shared_ptr<void> data);
	void* Get(const char* name);

	jsh::Mesh* CreateMesh(const char* name);
	jsh::RawData* CreateRawData(const char* name);
	jsh::Mesh* GetMesh(const char* name);
	jsh::RawData* GetRawData(const char* name);
	void RemoveMesh(const char* name);
	void RemoveRawData(const char* name);

#ifdef JSH_ENGINE

#ifdef JSH_IMGUI
	bool ShowMeshImGuiWindow(jsh::Mesh* mesh = nullptr);
	bool ShowRawDataImGuiWindow(jsh::RawData* rawData = nullptr);
#endif
	void ClearObjects();
#endif
}


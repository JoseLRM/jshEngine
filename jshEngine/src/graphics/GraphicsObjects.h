#pragma once

#include "GraphicsPrimitives.h"
#include "..//utils/dataStructures/vector.h"

namespace jsh {

	typedef uint32 Entity;
		
	// TODO: Material
	// TODO: Light

	// Shader
	struct Shader {
		VertexShader vs;
		PixelShader ps;
	};

	////////////////////////////RAWDATA//////////////////////////////
	typedef uint16 JSH_RAW_DATA;

	#define JSH_RAW_DATA_NONE			0
	#define JSH_RAW_DATA_POSITIONS		BIT(0)
	#define JSH_RAW_DATA_NORMALS		BIT(1)
	#define JSH_RAW_DATA_TEX_COORDS		BIT(2)
	#define JSH_RAW_DATA_INDICES		BIT(3)
	#define JSH_RAW_DATA_COLORS			BIT(4)

	class RawData {
		JSH_RAW_DATA m_DataFlags;
		uint8 m_CreationType = 0u;

		float* m_pPosData = nullptr;
		float* m_pNorData = nullptr;
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

		// getters
		inline bool IsCreated() const noexcept { return m_Created; }
		inline bool IsValid() const noexcept {
			return IsCreated() ? true : m_DataFlags != JSH_RAW_DATA_NONE &&
				(m_DataFlags & JSH_RAW_DATA_POSITIONS && m_DataFlags & JSH_RAW_DATA_NORMALS && m_DataFlags & JSH_RAW_DATA_INDICES);
		}
		inline JSH_RAW_DATA GetFlags() const noexcept { return m_DataFlags; }
		bool CreateInputLayout(jsh::InputLayout* il, jsh::VertexShader& vs) const noexcept;

		inline uint32 GetIndexCount() const noexcept { return m_IndexCount; }

	private:
		void CreateSolid();
		void CreateSimpleTex();
		void CreateSimpleCol();

	};	

	////////////////////////////MESH//////////////////////////////
	class Mesh {
		Texture m_DiffuseMap;
		SamplerState m_DiffuseSamplerState;
		Texture m_NormalMap;
		SamplerState m_NormalSamplerState;

		InputLayout m_InputLayout;
		VertexShader m_VShader;
		PixelShader m_PShader;

		bool m_EnabledDiffuseMap = false;
		bool m_EnabledNormalMap = false;

		bool m_Modified = true;

#ifdef JSH_ENGINE
	public:
#endif
		Mesh();
	public:
		RawData* rawData;

		inline void UpdatePrimitives() noexcept { if (m_Modified) Update(); }
		void Bind(jsh::CommandList cmd);

		void SetDiffuseMap(Texture tex, SamplerState* state = nullptr) noexcept;
		void SetNormalMap(Texture tex, SamplerState* state = nullptr) noexcept;

		void EnableDiffuseMap(bool enable) noexcept;
		void EnableNormalMap(bool enable) noexcept;

		bool HasDiffuseMap() const noexcept { return m_DiffuseMap.IsValid(); }
		bool HasNormalMap() const noexcept { return m_NormalMap.IsValid(); }

	private:
		void Update() noexcept;
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

}


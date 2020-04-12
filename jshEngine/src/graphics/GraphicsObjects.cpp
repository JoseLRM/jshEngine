#include "GraphicsObjects.h"

#include "..//Debug.h"
#include <map>
#include "Graphics.h"
#include "..//ecs/Scene.h"

using namespace jsh;

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

	bool RawData::CreateInputLayout(jsh::InputLayout* il, jsh::VertexShader& vs) const noexcept
	{
		if (m_CreationType == 0) return false;;

		switch (m_CreationType)
		{
		case 1: // solid
		{
			const JSH_INPUT_ELEMENT_DESC desc[] = {
				{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u},
				{"Normal", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 3 * sizeof(float), 0u}
			};
			jshGraphics::CreateInputLayout(desc, 2, vs, il);
		}
			return true;;
		case 2: // simple tex
		{
			const JSH_INPUT_ELEMENT_DESC desc[] = {
					{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u},
					{"Normal", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 3 * sizeof(float), 0u},
					{"TexCoord", 0, JSH_FORMAT_R32G32_FLOAT, 0, true, 6 * sizeof(float), 0u}
			};
			jshGraphics::CreateInputLayout(desc, 3, vs, il);
		}
			return true;;
		case 3: // simple col
		{
			const JSH_INPUT_ELEMENT_DESC desc[] = {
				{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u},
				{"Normal", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 3 * sizeof(float), 0u},
				{"Color", 0, JSH_FORMAT_R8G8B8A8_UNORM, 0, true, 6 * sizeof(float), 0u}
			};
			jshGraphics::CreateInputLayout(desc, 3, vs, il);
		}
			return true;
		case 4: // with tangents
		{
			const JSH_INPUT_ELEMENT_DESC desc[] = {
				{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u},
				{"Normal", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 3 * sizeof(float), 0u},
				{"TexCoord", 0, JSH_FORMAT_R32G32_FLOAT, 0, true, 6 * sizeof(float), 0u},
				{"Tangent", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 8 * sizeof(float), 0u},
				{"Bitangent", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 11 * sizeof(float), 0u}
			};
			jshGraphics::CreateInputLayout(desc, 5, vs, il);
		}
		return true;
		default:
			return false;;
		}
	}

	//////////////////////MESH/////////////////////////////////
	Mesh::Mesh() : rawData(nullptr), m_EnabledDiffuseMap(false), m_EnabledNormalMap(false) {}

	void Mesh::Bind(CommandList cmd)
	{
		rawData->Bind(cmd);
		if (m_EnabledDiffuseMap) {
			jshGraphics::BindTexture(m_DiffuseMap, 0u, JSH_SHADER_TYPE_PIXEL, cmd);
			if(m_DiffuseSamplerState.IsValid()) jshGraphics::BindSamplerState(m_DiffuseSamplerState, 0u, JSH_SHADER_TYPE_PIXEL, cmd);
		}

		if (m_EnabledNormalMap) {
			jshGraphics::BindTexture(m_NormalMap, 1u, JSH_SHADER_TYPE_PIXEL, cmd);
			if (m_NormalSamplerState.IsValid()) jshGraphics::BindSamplerState(m_NormalSamplerState, 1u, JSH_SHADER_TYPE_PIXEL, cmd);
		}

		jshGraphics::BindInputLayout(m_InputLayout, cmd);
		jshGraphics::BindVertexShader(m_VShader, cmd);
		jshGraphics::BindPixelShader(m_PShader, cmd);
	}

	void Mesh::Update() noexcept
	{
		JSH_RAW_DATA flags = rawData->GetFlags();

		m_Modified = false;

		if (flags & JSH_RAW_DATA_TEX_COORDS && flags & JSH_RAW_DATA_TANGENTS && m_EnabledDiffuseMap && m_EnabledNormalMap) {
			Shader* shader = (Shader*)jshGraphics::Get("NormalShader");
			m_VShader = shader->vs;
			m_PShader = shader->ps;
		}
		else if (flags & JSH_RAW_DATA_TEX_COORDS && m_EnabledDiffuseMap) {
			Shader* shader = (Shader*) jshGraphics::Get("SimpleTexShader");
			m_VShader = shader->vs;
			m_PShader = shader->ps;
		}
		else if (flags & JSH_RAW_DATA_COLORS) {
			Shader* shader = (Shader*)jshGraphics::Get("SimpleColShader");
			m_VShader = shader->vs;
			m_PShader = shader->ps;
		}
		else {
			Shader* shader = (Shader*)jshGraphics::Get("SolidShader");
			m_VShader = shader->vs;
			m_PShader = shader->ps;
		}

		if(!rawData->CreateInputLayout(&m_InputLayout, m_VShader)) return;
	}

	void Mesh::SetDiffuseMap(Texture tex, SamplerState* state) noexcept
	{
		m_DiffuseMap = tex;
		m_EnabledDiffuseMap = true;
		m_Modified = true;

		if (state && state->IsValid()) {
			m_DiffuseSamplerState = *state;
		}
	}
	void Mesh::SetNormalMap(Texture tex, SamplerState* state) noexcept
	{
		m_NormalMap = tex;
		m_EnabledNormalMap = true;
		m_Modified = true;

		if (state && state->IsValid()) {
			m_NormalSamplerState = *state;
		}
	}

	void Mesh::EnableDiffuseMap(bool enable) noexcept
	{
		if (enable == m_EnabledDiffuseMap) return;
		m_Modified = true;
		m_EnabledDiffuseMap = enable;
	}
	void Mesh::EnableNormalMap(bool enable) noexcept
	{
		if (enable == m_EnabledNormalMap) return;
		m_Modified = true;
		m_EnabledNormalMap = enable;
	}

	//////////////////////MODEL/////////////////////////////////
	Model::Model() : MeshNode() {}

	void Model::CreateEntity(jsh::Entity entity) noexcept
	{
		MeshComponent* comp = jshScene::GetComponent<MeshComponent>(entity);
		if (!comp) {
			jshScene::AddComponent(entity, jsh::MeshComponent());
		}

		AddNode(entity, this);
	}
	void Model::AddNode(jsh::Entity parent, MeshNode* node) noexcept
	{
		MeshComponent* meshComp = jshScene::GetComponent<MeshComponent>(parent);
		meshComp->mesh = node->mesh;

		uint32 cant = node->sons.size();
		if (cant == 0) return;

		jsh::vector<Entity> entities;
		jshScene::CreateSEntities(parent, cant, &entities, jsh::MeshComponent());

		for (uint32 i = 0; i < cant; ++i) {
			AddNode(entities[i], &sons[i]);
		}
	}

	//////////////////////FRAME BUFFER/////////////////////////////////
	DepthStencilState FrameBuffer::s_NullDST;
	DepthStencilState FrameBuffer::s_DepthDST;
	DepthStencilState FrameBuffer::s_StencilDST;
	DepthStencilState FrameBuffer::s_DepthStencilDST;

	void FrameBuffer::Initialize()
	{
		JSH_DEPTH_STENCIL_DESC nullDesc;
		nullDesc.DepthEnable = false;
		nullDesc.DepthWriteMask = JSH_DEPTH_WRITE_MASK_ALL;
		nullDesc.StencilEnable = false;
		jshGraphics::CreateDepthStencilState(&nullDesc, &s_NullDST);

		JSH_DEPTH_STENCIL_DESC depthDesc;
		depthDesc.DepthEnable = true;
		depthDesc.DepthFunc = JSH_COMPARISON_LESS;
		depthDesc.DepthWriteMask = JSH_DEPTH_WRITE_MASK_ALL;
		depthDesc.StencilEnable = false;
		jshGraphics::CreateDepthStencilState(&depthDesc, &s_DepthDST);
	}

	FrameBuffer::FrameBuffer() : m_Width(1080), m_Height(720) {}
	FrameBuffer::FrameBuffer(bool depthTest, bool stencilTest) : m_Width(1080), m_Height(720) {
		if (!depthTest && !stencilTest) m_DepthStateID = 0;
		else if (depthTest && !stencilTest) m_DepthStateID = 1;
		else if (!depthTest && stencilTest) m_DepthStateID = 2;
		else m_DepthStateID = 3;
	}
	FrameBuffer::FrameBuffer(uint32 width, uint32 height)
		: m_Width(width),
		m_Height(height), m_ConserveResolution(false) {}

	void FrameBuffer::Create(bool fromBackBuffer) {

		JSH_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize = 1u;
		texDesc.BindFlags = JSH_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0u;
		texDesc.Format = JSH_FORMAT_D24_UNORM_S8_UINT;
		texDesc.Width = 1080;
		texDesc.Height = 720;
		texDesc.MipLevels = 1u;
		texDesc.MiscFlags = 0u;
		texDesc.SampleDesc.Count = 1u;
		texDesc.SampleDesc.Quality = 0u;
		texDesc.Usage = JSH_USAGE_DEFAULT;

		jshGraphics::CreateTexture(&texDesc, nullptr, &m_Texture);

		if (fromBackBuffer) {
			jshGraphics::CreateRenderTargetViewFromBackBuffer(&m_RenderTarget);
		}
		else {
			JSH_TEXTURE2D_DESC rtvTexDesc;
			texDesc.ArraySize = 1u;
			texDesc.BindFlags = JSH_BIND_RENDER_TARGET;
			texDesc.CPUAccessFlags = 0u;
			texDesc.Format = JSH_FORMAT_D24_UNORM_S8_UINT;
			texDesc.Width = 1080;
			texDesc.Height = 720;
			texDesc.MipLevels = 1u;
			texDesc.MiscFlags = 0u;
			texDesc.SampleDesc.Count = 1u;
			texDesc.SampleDesc.Quality = 0u;
			texDesc.Usage = JSH_USAGE_DEFAULT;
			JSH_RENDER_TARGET_VIEW_DESC rtvDesc;
			rtvDesc.Format = JSH_FORMAT_D24_UNORM_S8_UINT;
			rtvDesc.ViewDimension = JSH_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0u;

			jshGraphics::CreateRenderTargetView(&rtvDesc, &rtvTexDesc, &m_RenderTarget);
		}
	}
	void FrameBuffer::Bind(jsh::CommandList cmd) const
	{
		jshGraphics::BindRenderTargetView(m_RenderTarget, m_Texture, cmd);
		BindDepthStencilState(cmd);
	}
	void FrameBuffer::Clear(CommandList cmd) const
	{
		jshGraphics::ClearRenderTargetView(m_RenderTarget, cmd);
		jshGraphics::ClearDepthStencilView(m_Texture, cmd);
	}

	void FrameBuffer::EnableDepthTest(jsh::CommandList cmd)
	{
		if (m_DepthStateID == 1 || m_DepthStateID == 3) return;
		m_DepthStateID = (m_DepthStateID == 2) ? 3 : 1;
		if (IsBounded()) BindDepthStencilState(cmd);
	}
	void FrameBuffer::DisableDepthTest(jsh::CommandList cmd)
	{
		if (m_DepthStateID == 0 || m_DepthStateID == 2) return;
		m_DepthStateID = (m_DepthStateID == 1) ? 0 : 2;
		if (IsBounded()) BindDepthStencilState(cmd);
	}
	void FrameBuffer::EnableStencilTest(jsh::CommandList cmd)
	{
		if (m_DepthStateID == 2 || m_DepthStateID == 3) return;
		m_DepthStateID = (m_DepthStateID == 1) ? 3 : 2;
		if (IsBounded()) BindDepthStencilState(cmd);
	}
	void FrameBuffer::DisableStencilTest(jsh::CommandList cmd)
	{
		if (m_DepthStateID == 0 || m_DepthStateID == 1) return;
		m_DepthStateID = (m_DepthStateID == 2) ? 0 : 3;
		if (IsBounded()) BindDepthStencilState(cmd);
	}
	void FrameBuffer::BindDepthStencilState(CommandList cmd) const
	{
		switch (m_DepthStateID)
		{
		case 0:
			jshGraphics::BindDepthStencilState(s_NullDST, cmd);
			break;
		case 1:
			jshGraphics::BindDepthStencilState(s_DepthDST, cmd);
			break;
		case 2:
			jshGraphics::BindDepthStencilState(s_StencilDST, cmd);
			break;
		case 3:
			jshGraphics::BindDepthStencilState(s_DepthStencilDST, cmd);
			break;
		}
	}
	bool FrameBuffer::IsBounded() const
	{
		return (m_RenderTarget.internalAllocation.get() == jshGraphics::GetRenderTargetView().internalAllocation.get());
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

	jsh::Mesh* CreateMesh(const char* name)
	{
		std::string n = name;
		Mesh* mesh = new Mesh();
		if (g_Mesh.find(n) == g_Mesh.end()) {
			g_Mesh[n] = mesh;
		}
		else {
			jshLogW("Duplicated mesh, name '%s'", name);
		}
		return mesh;
	}
	jsh::RawData* CreateRawData(const char* name)
	{
		std::string n = name;
		RawData* rawData = new RawData();
		if (g_RawData.find(n) == g_RawData.end()) {
			g_RawData[n] = rawData;
		}
		else {
			jshLogW("Duplicated rawData, name '%s'", name);
		}
		return rawData;
	}
	jsh::Mesh* GetMesh(const char* name)
	{
		auto it = g_Mesh.find(name);
		if (it == g_Mesh.end()) {
			return nullptr;
		}
		else {
			return (*it).second;
		}
	}
	jsh::RawData* GetRawData(const char* name)
	{
		auto it = g_RawData.find(name);
		if (it == g_RawData.end()) {
			return nullptr;
		}
		else {
			return (*it).second;
		}
	}

	void RemoveMesh(const char* name)
	{
		auto it = g_Mesh.find(name);
		if (it != g_Mesh.end()) {
			delete (*it).second;
			g_Mesh.erase(name);
		}
	}
	void RemoveRawData(const char* name)
	{
		auto it = g_RawData.find(name);
		if (it != g_RawData.end()) {
			delete (*it).second;
			g_RawData.erase(name);
		}
	}

}
#include "GraphicsObjects.h"

#include "..//Debug.h"
#include <map>
#include "Graphics.h"

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
			CreateSimpleTex();
			return;
		}

		CreateSolid();
	}

	void RawData::Bind(jsh::CommandList cmd)
	{
		assert(IsCreated());
		
		jshGraphics::BindVertexBuffer(m_VertexBuffer, 0, cmd);
		jshGraphics::BindIndexBuffer(m_IndexBuffer, cmd);
		jshGraphics::BindInputLayout(m_InputLayout, cmd);
		jshGraphics::BindVertexShader(m_VS, cmd);
		jshGraphics::BindPixelShader(m_PS, cmd);
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

	void RawData::SetTextureCoords(float* coords)
	{
		m_pTexCoordsData = coords;
		if (coords) m_DataFlags |= JSH_RAW_DATA_TEX_COORDS;
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

		const Shader& shader = *(Shader*)jshGraphics::Get("SolidShader");

		const JSH_INPUT_ELEMENT_DESC desc[] = {
				{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u},
				{"Normal", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 3 * sizeof(float), 0u}
		};

		m_VS = shader.vs;
		m_PS = shader.ps;

		jshGraphics::CreateInputLayout(desc, 2, m_VS, &m_InputLayout);

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

		const Shader& shader = *(Shader*)jshGraphics::Get("SimpleTexShader");

		const JSH_INPUT_ELEMENT_DESC desc[] = {
				{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u},
				{"Normal", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 3 * sizeof(float), 0u},
				{"TexCoord", 0, JSH_FORMAT_R32G32_FLOAT, 0, true, 6 * sizeof(float), 0u}
		};

		m_VS = shader.vs;
		m_PS = shader.ps;

		jshGraphics::CreateInputLayout(desc, 3, m_VS, &m_InputLayout);

		delete[] vData;
	}

	//////////////////////MESH/////////////////////////////////
	Mesh::Mesh() : rawData(nullptr), diffuseMap() {}
	Mesh::Mesh(RawData* rawData) : rawData(rawData), diffuseMap() {}
	Mesh::Mesh(RawData* rawData, Texture diffuseMap) : rawData(rawData), diffuseMap(diffuseMap) {}

	void Mesh::Bind(CommandList cmd) noexcept
	{
		if (rawData) rawData->Bind(cmd);
		else return;
		if (diffuseMap.IsValid()) jshGraphics::BindTexture(diffuseMap, 0u, JSH_SHADER_TYPE_PIXEL, cmd);
	}

	//////////////////////MODEL/////////////////////////////////
	Model::Model() : meshes() {}

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

}
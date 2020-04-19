#include "GraphicsObjects.h"

#include "Debug.h"
#include <map>
#include "Graphics.h"
#include "Renderer.h"
#include "Scene.h"

using namespace jsh;

namespace jsh {

	/////////////////////////////INSTANCE BUFFER//////////////////////////
	void InstanceBuffer::Create()
	{
		XMMATRIX aux;
		JSH_BUFFER_DESC desc;
		desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(XMMATRIX);
		desc.CPUAccessFlags = 0u;
		desc.MiscFlags = 0u;
		desc.StructureByteStride = 0u;
		desc.Usage = JSH_USAGE_DEFAULT;
		JSH_SUBRESOURCE_DATA sdata;
		sdata.pSysMem = &aux;
		jshGraphics::CreateResource(&desc, &sdata, &m_Buffer);
	}
	void InstanceBuffer::Bind(JSH_SHADER_TYPE shaderType, jsh::CommandList cmd) const
	{
		jshGraphics::BindConstantBuffer(m_Buffer, JSH_GFX_SLOT_CBUFFER_INSTANCE, shaderType, cmd);
	}
	void InstanceBuffer::UpdateBuffer(XMMATRIX* tm, jsh::CommandList cmd)
	{
		jshGraphics::UpdateConstantBuffer(m_Buffer, tm, cmd);
	}

	////////////////////////////MATERIAL///////////////////////////
	void Material::Create()
	{
		JSH_BUFFER_DESC desc;
		desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(float) * 4;
		desc.CPUAccessFlags = 0u;
		desc.MiscFlags = 0u;
		desc.StructureByteStride = 0u;
		desc.Usage = JSH_USAGE_DEFAULT;
		JSH_SUBRESOURCE_DATA sres;
		sres.pSysMem = &m_SpecularIntensity;
		jshGraphics::CreateResource(&desc, &sres, &m_Buffer);
	}

	void Material::Bind(CommandList cmd, JSH_SHADER_TYPE shaderType)
	{
		if (m_Modified) {
			struct alignas(16) {
				float specularIntensity;
				float shininess;
			} data;

			data.specularIntensity = m_SpecularIntensity;
			data.shininess = m_Shininess;

			jshGraphics::UpdateConstantBuffer(m_Buffer, &data, cmd);
			m_Modified = false;
		}
		jshGraphics::BindConstantBuffer(m_Buffer, JSH_GFX_SLOT_CBUFFER_MATERIAL, shaderType, cmd);
	}

	////////////////////////////TEXTURE///////////////////////////
	void Texture::Bind(JSH_SHADER_TYPE shaderType, jsh::CommandList cmd) const
	{
		switch (type)
		{
		case JSH_TEXTURE_DIFFUSE_MAP:
			jshGraphics::BindTexture(resource, JSH_GFX_SLOT_TEXTURE2D_DIFFUSE, shaderType, cmd);
			break;
		case JSH_TEXTURE_NORMAL_MAP:
			jshGraphics::BindTexture(resource, JSH_GFX_SLOT_TEXTURE2D_NORMAL, shaderType, cmd);
			break;
		case JSH_TEXTURE_SPECULAR_MAP:
			jshGraphics::BindTexture(resource, JSH_GFX_SLOT_TEXTURE2D_SPECULAR, shaderType, cmd);
			break;
		case JSH_TEXTURE_ATLAS:
			break;
		}
	}

	////////////////////////////STATE///////////////////////////
	void RenderState::SetDefault() noexcept
	{
		uint32 i = 0u;
		uint32 j = 0u;

		constexpr uint32 SHADER_TYPES_COUNT = 2;

		// buffers
		for (; i < JSH_GFX_VERTEX_BUFFERS_COUNT; ++i) {
			vertexBuffer[i].internalAllocation.reset();
		}
		for (i = 0; i < SHADER_TYPES_COUNT; ++i) {
			for (; j < JSH_GFX_CONSTANT_BUFFERS_COUNT; ++j) {
				constantBuffer[i][j].internalAllocation.reset();
			}
		}
		indexBuffer.internalAllocation.reset();

		// shaders
		vertexShader.internalAllocation.reset();
		pixelShader.internalAllocation.reset();

		inputLayout.internalAllocation.reset();

		// textures
		for (i = 0; i < SHADER_TYPES_COUNT; ++i) {
			for (j = 0; j < JSH_GFX_TEXTURES_COUNT; ++j) {
				texture[i][j].internalAllocation.reset();
			}
		}

		// states
		for (i = 0; i < SHADER_TYPES_COUNT; ++i) {
			for (j = 0; j < JSH_GFX_SAMPLER_STATES_COUNT; ++j) {
				samplerState[i][j] = jshRenderer::primitives::GetDefaultSamplerState();
			}
		}

		depthStencilState = jshRenderer::primitives::GetDefaultDepthStencilState();
		blendState.internalAllocation.reset();
		rasterizerState.internalAllocation.reset();

		renderTargetView[0] = jshRenderer::primitives::GetOffscreenRenderTargetView();
		for (i = 1; i < JSH_GFX_RENDER_TARGETS_COUNT; ++i) {
			renderTargetView[i].internalAllocation.reset();
		}

		// viewports
		viewport[0] = jshRenderer::primitives::GetDefaultViewport();
		for (i = 1; i < JSH_GFX_VIEWPORTS_COUNT; ++i) {
			viewport[i].internalAllocation.reset();
		}
	}
	void RenderState::Bind() const
	{
		
	}

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
		jshGraphics::CreateResource(&vertexDesc, &vertexSData, &m_VertexBuffer);

		JSH_BUFFER_DESC indexDesc;
		indexDesc.BindFlags = JSH_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = m_IndexCount * sizeof(uint32);
		indexDesc.CPUAccessFlags = 0u;
		indexDesc.MiscFlags = 0u;
		indexDesc.StructureByteStride = sizeof(uint32);
		indexDesc.Usage = JSH_USAGE_IMMUTABLE;
		JSH_SUBRESOURCE_DATA indexSData;
		indexSData.pSysMem = m_pIndexData;
		jshGraphics::CreateResource(&indexDesc, &indexSData, &m_IndexBuffer);

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
		jshGraphics::CreateResource(&vertexDesc, &vertexSData, &m_VertexBuffer);

		JSH_BUFFER_DESC indexDesc;
		indexDesc.BindFlags = JSH_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = m_IndexCount * sizeof(uint32);
		indexDesc.CPUAccessFlags = 0u;
		indexDesc.MiscFlags = 0u;
		indexDesc.StructureByteStride = sizeof(uint32);
		indexDesc.Usage = JSH_USAGE_IMMUTABLE;
		JSH_SUBRESOURCE_DATA indexSData;
		indexSData.pSysMem = m_pIndexData;
		jshGraphics::CreateResource(&indexDesc, &indexSData, &m_IndexBuffer);

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
		jshGraphics::CreateResource(&vertexDesc, &vertexSData, &m_VertexBuffer);

		JSH_BUFFER_DESC indexDesc;
		indexDesc.BindFlags = JSH_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = m_IndexCount * sizeof(uint32);
		indexDesc.CPUAccessFlags = 0u;
		indexDesc.MiscFlags = 0u;
		indexDesc.StructureByteStride = sizeof(uint32);
		indexDesc.Usage = JSH_USAGE_IMMUTABLE;
		JSH_SUBRESOURCE_DATA indexSData;
		indexSData.pSysMem = m_pIndexData;
		jshGraphics::CreateResource(&indexDesc, &indexSData, &m_IndexBuffer);

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
		jshGraphics::CreateResource(&vertexDesc, &vertexSData, &m_VertexBuffer);

		JSH_BUFFER_DESC indexDesc;
		indexDesc.BindFlags = JSH_BIND_INDEX_BUFFER;
		indexDesc.ByteWidth = m_IndexCount * sizeof(uint32);
		indexDesc.CPUAccessFlags = 0u;
		indexDesc.MiscFlags = 0u;
		indexDesc.StructureByteStride = sizeof(uint32);
		indexDesc.Usage = JSH_USAGE_IMMUTABLE;
		JSH_SUBRESOURCE_DATA indexSData;
		indexSData.pSysMem = m_pIndexData;
		jshGraphics::CreateResource(&indexDesc, &indexSData, &m_IndexBuffer);

		assert(m_VertexBuffer.IsValid());
		assert(m_IndexBuffer.IsValid());

		delete[] vData;
	}

	//////////////////////MESH/////////////////////////////////
	void Mesh::Bind(CommandList cmd)
	{
		m_RawData->Bind(cmd);

		if (m_DiffuseMapEnabled)	m_DiffuseMap->Bind(JSH_SHADER_TYPE_PIXEL, cmd);
		if (m_NormalMapEnabled)		m_NormalMap->Bind(JSH_SHADER_TYPE_PIXEL, cmd);
		if (m_SpecularMapEnabled)	m_SpecularMap->Bind(JSH_SHADER_TYPE_PIXEL, cmd);

		jshGraphics::BindInputLayout(m_Shader->inputLayout, cmd);
		jshGraphics::BindVertexShader(m_Shader->vs, cmd);
		jshGraphics::BindPixelShader(m_Shader->ps, cmd);
	}

	void Mesh::SetTexture(Texture* texture) noexcept
	{
		switch (texture->type)
		{
		case JSH_TEXTURE_DIFFUSE_MAP:
			if (texture->resource.IsValid()) {
				m_DiffuseMap = texture;
				m_DiffuseMapEnabled = true;
				m_Modified = true;
			}
			return;
		case JSH_TEXTURE_NORMAL_MAP:
			if (texture->resource.IsValid()) {
				m_NormalMap = texture;
				m_NormalMapEnabled = true;
				m_Modified = true;
			}
			return;
		case JSH_TEXTURE_SPECULAR_MAP:
			if (texture->resource.IsValid()) {
				m_SpecularMap = texture;
				m_SpecularMapEnabled = true;
				m_Modified = true;
			}
			return;
		default:
			jshLogW("Invalid texture type");
			return;
		}
	}
	void Mesh::EnableTexture(JSH_TEXTURE_TYPE type, bool enable) noexcept
	{
		switch (type)
		{
		case JSH_TEXTURE_DIFFUSE_MAP:
			if (enable && m_DiffuseMap) m_DiffuseMapEnabled = true;
			else m_DiffuseMapEnabled = false;
			m_Modified = true;
			return;
		case JSH_TEXTURE_NORMAL_MAP:
			if (enable && m_NormalMap) m_NormalMapEnabled = true;
			else m_NormalMapEnabled = false;
			m_Modified = true;
			return;
		case JSH_TEXTURE_SPECULAR_MAP:
			if (enable && m_SpecularMap) m_SpecularMapEnabled = true;
			else m_SpecularMapEnabled = false;
			m_Modified = true;
			return;
		default:
			jshLogW("Invalid texture type");
			return;
		}
	}
	bool Mesh::HasTexture(JSH_TEXTURE_TYPE type)
	{
		switch (type)
		{
		case JSH_TEXTURE_DIFFUSE_MAP:
			return m_DiffuseMapEnabled;
		case JSH_TEXTURE_NORMAL_MAP:
			return m_NormalMapEnabled;
		case JSH_TEXTURE_SPECULAR_MAP:
			return m_SpecularMapEnabled;
		default:
			jshLogW("Invalid texture type");
			return false;
		}
	}

	void Mesh::SetRawData(RawData* rawData) noexcept
	{
		assert(rawData != nullptr);
		m_RawData = rawData;
		m_Modified = true;
	}
	void Mesh::SetMaterial(Material* material) noexcept
	{
		assert(material != nullptr);
		m_Material = material;
	}
	void Mesh::SetShader(Shader* shader) noexcept
	{
		assert(shader != nullptr);
		m_Shader = shader;
		m_Modified = true;
	}

	void Mesh::UpdatePrimitives() noexcept
	{
		if (m_Modified) {

			m_Modified = false;

			RawDataFlags flags = m_RawData->GetFlags();

			if (flags & JSH_RAW_DATA_TEX_COORDS && flags & JSH_RAW_DATA_TANGENTS && (m_SpecularMapEnabled || m_NormalMapEnabled)) {
				m_Shader = jshGraphics::GetShader("Normal");
			}
			else if (flags & JSH_RAW_DATA_TEX_COORDS && m_DiffuseMapEnabled) {
				m_Shader = jshGraphics::GetShader("SimpleTex");
			}
			else if (flags & JSH_RAW_DATA_COLORS) {
				m_Shader = jshGraphics::GetShader("SimpleCol");
			}
			else {
				m_Shader = jshGraphics::GetShader("Solid");
			}
		}
	}

	Texture* Mesh::GetTexture(JSH_TEXTURE_TYPE type) const noexcept
	{
		switch (type)
		{
		case JSH_TEXTURE_DIFFUSE_MAP:
			return m_DiffuseMap;
		case JSH_TEXTURE_NORMAL_MAP:
			return m_NormalMap;
		case JSH_TEXTURE_SPECULAR_MAP:
			return m_SpecularMap;
		default:
			jshLogW("Invalid texture type");
			return nullptr;
		}
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
			jshScene::GetComponent<NameComponent>(parent)->name = node->name;
			jshScene::GetTransform(meshComp->entityID) = node->transform;
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
	jsh::Material* CreateMaterial(const char* name)
	{
		std::string n = name;
		Material* material = new Material();
		if (g_Material.find(n) == g_Material.end()) {
			g_Material[n] = material;
		}
		else {
			jshLogW("Duplicated material, name '%s'", name);
		}
		return material;
	}
	jsh::Shader* CreateShader(const char* name)
	{
		std::string n = name;
		Shader* shader = new Shader();
		if (g_Shader.find(n) == g_Shader.end()) {
			g_Shader[n] = shader;
		}
		else {
			jshLogW("Duplicated shader, name '%s'", name);
		}
		return shader;
	}
	jsh::Texture* CreateTexture(const char* name)
	{
		std::string n = name;
		Texture* texture = new Texture();
		if (g_Texture.find(n) == g_Texture.end()) {
			g_Texture[n] = texture;
		}
		else {
			jshLogW("Duplicated texture, name '%s'", name);
		}
		return texture;
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
	jsh::Material* GetMaterial(const char* name)
	{
		auto it = g_Material.find(name);
		if (it == g_Material.end()) {
			return nullptr;
		}
		else {
			return (*it).second;
		}
	}
	jsh::Shader* GetShader(const char* name)
	{
		auto it = g_Shader.find(name);
		if (it == g_Shader.end()) {
			return nullptr;
		}
		else {
			return (*it).second;
		}
	}
	jsh::Texture* GetTexture(const char* name)
	{
		auto it = g_Texture.find(name);
		if (it == g_Texture.end()) {
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
	void RemoveMaterial(const char* name)
	{
		auto it = g_Material.find(name);
		if (it != g_Material.end()) {
			delete (*it).second;
			g_Material.erase(name);
		}
	}
	void RemoveShader(const char* name)
	{
		auto it = g_Shader.find(name);
		if (it != g_Shader.end()) {
			delete (*it).second;
			g_Shader.erase(name);
		}
	}
	void RemoveTexture(const char* name)
	{
		auto it = g_Texture.find(name);
		if (it != g_Texture.end()) {
			delete (*it).second;
			g_Texture.erase(name);
		}
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

	const char* g_SelectedMesh = nullptr;
	bool ShowMeshImGuiWindow(jsh::Mesh* mesh)
	{
		for (auto& it : g_Mesh) {
			if (it.second == mesh) {
				g_SelectedMesh = it.first.c_str();
				break;
			}
		}

		bool result = true;
		if (ImGui::Begin("Meshes")) {

			ImGui::Columns(2);

			for (auto& it : g_Mesh) {

				const char* cstr = it.first.c_str();

				bool selected = cstr == g_SelectedMesh;

				if (ImGui::Selectable(it.first.c_str(), selected)) {
					g_SelectedMesh = cstr;
				}
			}

			ImGui::NextColumn();

			if (g_SelectedMesh != nullptr) {
				ImGui::Text(g_SelectedMesh);

				Mesh* mesh = g_Mesh[g_SelectedMesh];
				
				bool diffuse = mesh->HasTexture(JSH_TEXTURE_DIFFUSE_MAP);
				bool normal = mesh->HasTexture(JSH_TEXTURE_NORMAL_MAP);
				bool specular = mesh->HasTexture(JSH_TEXTURE_SPECULAR_MAP);

				ImGui::Checkbox("DiffuseMapping", &diffuse);
				mesh->EnableTexture(JSH_TEXTURE_DIFFUSE_MAP, diffuse);

				ImGui::Checkbox("NormalMapping", &normal);
				mesh->EnableTexture(JSH_TEXTURE_NORMAL_MAP, normal);

				ImGui::Checkbox("SpecularMapping", &specular);
				mesh->EnableTexture(JSH_TEXTURE_SPECULAR_MAP, specular);

				mesh->UpdatePrimitives();
			}

			ImGui::NextColumn();

			if (ImGui::Button("Close")) result = false;
		}
		ImGui::End();
		return result;
	}

	const char* g_SelectedRawData = nullptr;
	bool ShowRawDataImGuiWindow(jsh::RawData* rawData)
	{
		for (auto& it : g_RawData) {
			if (it.second == rawData) {
				g_SelectedRawData = it.first.c_str();
				break;
			}
		}

		bool result = true;
		if (ImGui::Begin("RawData")) {

			ImGui::Columns(2);

			for (auto& it : g_RawData) {
				
				const char* cstr = it.first.c_str();

				bool selected = cstr == g_SelectedRawData;

				if (ImGui::Selectable(it.first.c_str(), selected)) {
					g_SelectedRawData = cstr;
				}
			}

			ImGui::NextColumn();

			if (g_SelectedRawData != nullptr) {
				ImGui::Text(g_SelectedRawData);

				RawData* rawData = g_RawData[g_SelectedRawData];

				ImGui::Text((std::string("Index Count: ") + std::to_string(rawData->GetIndexCount())).c_str());
				ImGui::Text((std::string("Triangles: ") + std::to_string(rawData->GetIndexCount() / 3)).c_str());

			}

			ImGui::NextColumn();

			if (ImGui::Button("Close")) result = false;
		}
		ImGui::End();
		return result;
	}
#endif

}
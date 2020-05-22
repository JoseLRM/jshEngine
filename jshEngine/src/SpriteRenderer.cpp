#include "common.h"

#include "SpriteRenderer.h"
#include "Technique.h"

namespace jsh {

	VertexShader SpriteRenderer::s_VertexShader;
	PixelShader SpriteRenderer::s_PixelShader;
	InputLayout SpriteRenderer::s_InputLayout;
	Buffer SpriteRenderer::s_VertexBuffer;
	JSH_TOPOLOGY SpriteRenderer::s_Topology;

	SpriteRenderer::SpriteRenderer(RenderTargetView& rtv, Viewport& vp)
	{
		AddAttachment(reinterpret_cast<GfxPrimitive*>(&rtv));
		m_pViewport = &vp;
		m_ViewProjectionMatrix = XMMatrixIdentity();
	}

	void SpriteRenderer::Begin(Camera& camera)
	{
		m_Instances.clear();
		m_ViewProjectionMatrix = camera.GetViewMatrix() * camera.GetProjectionMatrix();
	}
	void SpriteRenderer::End()
	{
		std::sort(m_Instances.begin(), m_Instances.end());
	}

	void SpriteRenderer::DrawScene()
	{
		auto& spritesList = jshScene::_internal::GetComponentsList()[SpriteComponent::ID];
		
		const uint32 size = uint32(SpriteComponent::SIZE);

		for (uint32 i = 0; i < spritesList.size(); i += size) {
			SpriteComponent* spr = reinterpret_cast<SpriteComponent*>(&spritesList[i]);
			Transform& trans = jshScene::GetTransform(spr->entity);

			GuiComponent* gui = jshScene::GetComponent<GuiComponent>(spr->entity);

			float layer = trans.GetWorldPosition().z;

			if (!gui) {

				if (spr->sprite.texture) {
					DrawSprite(trans.GetWorldMatrix(), spr->sprite, spr->color, layer);
				}
				else {
					DrawQuad(trans.GetWorldMatrix(), spr->color, layer);
				}
			}
			//TODO: Gui Renderer
			else {
				m_Instances.emplace_back(spr->sprite, spr->color, XMMatrixTranspose(trans.GetWorldMatrix()), layer);
			}
		}
	}

	void SpriteRenderer::ReserveInstances(uint32 count)
	{
		m_Instances.reserve(count);
	}

	void SpriteRenderer::DrawQuad(const vec2& position, const vec2& size, Color4b color, float layer)
	{
		XMMATRIX matrix = XMMatrixTranspose(XMMatrixScaling(size.x, size.y, 0.f) * XMMatrixTranslation(position.x, position.y, 0.f) * m_ViewProjectionMatrix);
		m_Instances.emplace_back(color, matrix, layer);
	}
	void SpriteRenderer::DrawQuad(const vec2& position, const vec2& size, float rotation, Color4b color, float layer)
	{
		XMMATRIX matrix = XMMatrixTranspose(XMMatrixScaling(size.x, size.y, 0.f) * XMMatrixRotationZ(rotation) * XMMatrixTranslation(position.x, position.y, 0.f) * m_ViewProjectionMatrix);
		m_Instances.emplace_back(color, matrix, layer);
	}
	void SpriteRenderer::DrawQuad(const XMMATRIX& tm, Color4b color, float layer)
	{
		m_Instances.emplace_back(color, XMMatrixTranspose(tm * m_ViewProjectionMatrix), layer);
	}
		 
	void SpriteRenderer::DrawSprite(const vec2& position, const vec2& size, const Sprite& sprite, Color4b color, float layer)
	{
		XMMATRIX matrix = XMMatrixTranspose(XMMatrixScaling(size.x, size.y, 0.f) * XMMatrixTranslation(position.x, position.y, 0.f) * m_ViewProjectionMatrix);
		m_Instances.emplace_back(sprite, color, matrix, layer);
	}
	void SpriteRenderer::DrawSprite(const vec2& position, const vec2& size, float rotation, const Sprite& sprite, Color4b color, float layer)
	{
		XMMATRIX matrix = XMMatrixTranspose(XMMatrixScaling(size.x, size.y, 0.f) * XMMatrixRotationZ(rotation) * XMMatrixTranslation(position.x, position.y, 0.f) * m_ViewProjectionMatrix);
		m_Instances.emplace_back(sprite, color, matrix, layer);
	}
	void SpriteRenderer::DrawSprite(const XMMATRIX& tm, const Sprite& sprite, Color4b color, float layer)
	{
		m_Instances.emplace_back(sprite, color, XMMatrixTranspose(tm * m_ViewProjectionMatrix), layer);
	}


	void SpriteRenderer::Initialize()
	{
		m_InstanceBuffer.Create(JSH_GFX_BATCH_COUNT * JSH_GFX_SPRITE_INSTANCE_SIZE);

		// Static initialization
		if (!s_VertexBuffer.IsValid()) {
			s_Topology = JSH_TOPOLOGY_TRIANGLE_STRIP;

			// Vertex Buffer
			{
				float data[] = {
					-0.5f,  0.5f,
					 0.5f,  0.5f,
					-0.5f, -0.5f,
					 0.5f, -0.5f
				};

				JSH_BUFFER_DESC desc;
				desc.BindFlags = JSH_BIND_VERTEX_BUFFER;
				desc.ByteWidth = 8 * sizeof(float);
				desc.CPUAccessFlags = 0u;
				desc.MiscFlags = 0u;
				desc.StructureByteStride = 2 * sizeof(float);
				desc.Usage = JSH_USAGE_IMMUTABLE;

				JSH_SUBRESOURCE_DATA sdata;
				sdata.pSysMem = data;

				jshGraphics::CreateBuffer(&desc, &sdata, &s_VertexBuffer);
			}

			// Shaders
			{
				jshGraphics::CreateVertexShader(L"SpriteVertex.cso", &s_VertexShader);
				jshGraphics::CreatePixelShader(L"SpritePixel.cso", &s_PixelShader);
			}

			// Input Layout
			{
				const JSH_INPUT_ELEMENT_DESC elements[] = {
					{"Position", 0, JSH_FORMAT_R32G32_FLOAT, 0u, true, 0u * sizeof(float), 0u},

					{"TM", 0, JSH_FORMAT_R32G32B32A32_FLOAT, 1u, false, 0u * sizeof(float), 1u},
					{"TM", 1, JSH_FORMAT_R32G32B32A32_FLOAT, 1u, false, 4u * sizeof(float), 1u},
					{"TM", 2, JSH_FORMAT_R32G32B32A32_FLOAT, 1u, false, 8u * sizeof(float), 1u},
					{"TM", 3, JSH_FORMAT_R32G32B32A32_FLOAT, 1u, false, 12u * sizeof(float), 1u},
					{"TexCoord", 0, JSH_FORMAT_R32G32B32A32_FLOAT, 1u, false, 16u * sizeof(float), 1u},
					{"Color", 0, JSH_FORMAT_R8G8B8A8_UNORM, 1u, false, 20u * sizeof(float), 1u},
					{"Texture", 0, JSH_FORMAT_R32_UINT, 1u, false, 21u * sizeof(float), 1u}
				};

				jshGraphics::CreateInputLayout(elements, 8u, s_VertexShader, &s_InputLayout);
			}
		}
	}
	void SpriteRenderer::Update()
	{

	}
	void SpriteRenderer::Render(CommandList cmd)
	{
		RenderTargetView& rtv = *reinterpret_cast<RenderTargetView*>(GetAttachment(0));

		jshGraphics::BindRenderTargetView(rtv, cmd);
		jshGraphics::BindViewport(*m_pViewport, 0u, cmd);

		for (uint32 i = 0; i < JSH_GFX_SAMPLER_STATES_COUNT; ++i) jshGraphics::BindSamplerState(jshGraphics::primitives::GetDefaultSamplerState(), i, JSH_SHADER_TYPE_PIXEL, cmd);

		//draw 
		auto& instances = m_Instances;

		jshGraphics::BindBlendState(jshGraphics::primitives::GetTransparentBlendState(), cmd);
		jshGraphics::BindDepthStencilState(jshGraphics::primitives::GetDisabledDepthStencilState(), 0u, cmd);

		jshGraphics::BindVertexShader(s_VertexShader, cmd);
		jshGraphics::BindPixelShader(s_PixelShader, cmd);
		jshGraphics::BindInputLayout(s_InputLayout, cmd);
		jshGraphics::SetTopology(s_Topology, cmd);

		const uint32 strides[] = {
					jshGraphics::GetBufferDesc(s_VertexBuffer).StructureByteStride,
					JSH_GFX_SPRITE_INSTANCE_SIZE
		};

		const Buffer* buffers[] = {
					&s_VertexBuffer,
					&m_InstanceBuffer.GetBuffer()
		};

		struct DrawCallData {
			uint32 beginIndex;
			uint32 endIndex;
			Texture* texture;

			DrawCallData(uint32 begin, uint32 end, Texture* tex) : beginIndex(begin), endIndex(end), texture(tex) {}
		};
		std::vector<DrawCallData> drawCallData;
		constexpr uint32 maxIndex = JSH_GFX_BATCH_COUNT * JSH_GFX_SPRITE_INSTANCE_SIZE;
		
		for (uint32 i = 0; i < instances.size();) {

			Texture* texture = instances[i].sprite.texture;
			uint32 begin = 0u;
			uint32 indexCount = 0u;
			BOOL hasTexture = texture ? TRUE : FALSE;

			for (; i < instances.size(); ++i) {

				if (indexCount >= maxIndex) break;

				auto& instance = instances[i];
				if (texture != instance.sprite.texture) {
					drawCallData.emplace_back(begin, indexCount, texture);

					begin = indexCount;
					texture = instance.sprite.texture;
					hasTexture = texture ? TRUE : FALSE;
				}

				// Set instance data
				{
					memcpy(&m_InstanceData[indexCount], &instance.tm, sizeof(XMMATRIX));
					memcpy(&m_InstanceData[indexCount + sizeof(XMMATRIX)], &instance.sprite.coords, sizeof(vec4));
					memcpy(&m_InstanceData[indexCount + sizeof(XMMATRIX) + sizeof(vec4)], &instance.color, sizeof(Color4b));
					memcpy(&m_InstanceData[indexCount + sizeof(XMMATRIX) + sizeof(vec4) + sizeof(Color4b)], &hasTexture, sizeof(BOOL));
				}
				indexCount += JSH_GFX_SPRITE_INSTANCE_SIZE;

			}
			drawCallData.emplace_back(begin, indexCount, texture);

			m_InstanceBuffer.Update(m_InstanceData, indexCount, cmd);

			for (uint32 j = 0; j < drawCallData.size(); ++j) {
				texture = drawCallData[j].texture;

				uint32 beginIndex = drawCallData[j].beginIndex;
				uint32 endIndex = drawCallData[j].endIndex;

				if (texture) {
					jshGraphics::BindTexture(texture->resource, JSH_GFX_SLOT_TEXTURE2D_FREE0, JSH_SHADER_TYPE_PIXEL, cmd);
					jshGraphics::BindSamplerState(*texture->samplerState, JSH_GFX_SLOT_SAMPLER_FREE0, JSH_SHADER_TYPE_PIXEL, cmd);
				}

				const uint32 offsets[] = {
					0u, beginIndex
				};

				jshGraphics::BindVertexBuffers(buffers, 0u, 2, strides, offsets, cmd);

				jshGraphics::DrawInstanced(4, (endIndex - beginIndex) / JSH_GFX_SPRITE_INSTANCE_SIZE, 0u, 0u, cmd);
			}
			drawCallData.clear();

		}
	}
	void SpriteRenderer::Close()
	{
		// Static Release
		if (s_VertexBuffer.IsValid()) {
			s_VertexShader.Release();
			s_PixelShader.Release();
			s_InputLayout.Release();
			s_VertexBuffer.Release();
		}
	}

}
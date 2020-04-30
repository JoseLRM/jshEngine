#include "Rendering2DPass.h"

namespace jsh {

	void Rendering2DPass::Create()
	{
		// SPRITE INSTANCES BUFFER
		{
			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_VERTEX_BUFFER;
			desc.ByteWidth = JSH_GFX_SPRITE_BATCH_COUNT * sizeof(SpriteInstance);
			desc.CPUAccessFlags = JSH_CPU_ACCESS_WRITE;
			desc.StructureByteStride = sizeof(SpriteInstance);
			desc.Usage = JSH_USAGE_DYNAMIC;
			desc.MiscFlags = 0u;

			jshGraphics::CreateBuffer(&desc, nullptr, &m_SpriteInstances);
		}
	}

	void Rendering2DPass::Load()
	{

	}

	void Rendering2DPass::Render(CommandList cmd)
	{
		if (m_pSpriteBatch == nullptr) return;

		jshGraphics::BindRenderTargetView(jshGraphics::primitives::GetOffscreenRenderTargetView(), cmd);
		jshGraphics::BindViewport(jshGraphics::primitives::GetDefaultViewport(), 0u, cmd);

		jshGraphics::BindVertexBuffer(jshGraphics::primitives::GetQuadBuffer(), 0u, cmd);
		jshGraphics::BindVertexBuffer(m_SpriteInstances, 1u, cmd);

		jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLE_STRIP, cmd);

		Shader* shader = jshGraphics::GetShader("Sprite");
		jshGraphics::BindVertexShader(shader->vs, cmd);
		jshGraphics::BindPixelShader(shader->ps, cmd);
		jshGraphics::BindInputLayout(shader->inputLayout, cmd);

		jshGraphics::BindBlendState(jshGraphics::primitives::GetTransparentBlendState(), cmd);

		jshGraphics::BindConstantBuffer(m_pRenderGraph->GetCameraBuffer(), JSH_GFX_SLOT_CBUFFER_CAMERA, JSH_SHADER_TYPE_VERTEX, cmd);

		for (uint32 i = 0; i < JSH_GFX_TEXTURES_COUNT; ++i) {
			jshGraphics::BindSamplerState(jshGraphics::primitives::GetDefaultSamplerState(), i, JSH_SHADER_TYPE_PIXEL, cmd);
		}

		SpriteInstance* ptr;
		size_t size = 0u;
		m_pSpriteBatch->FillPoolAndBindTextures(&size, m_SpriteData, cmd);

		while (size > 0) {
			jshGraphics::UpdateBuffer(m_SpriteInstances, m_SpriteData, size * sizeof(SpriteData), cmd);
			jshGraphics::DrawInstanced(4, size, 0, 0, cmd);
			m_pSpriteBatch->FillPoolAndBindTextures(&size, m_SpriteData, cmd);
		}
	}
}
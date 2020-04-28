#include "BloomEffect.h"

#include "Renderer.h"
#include "PostProcess.h"

namespace jsh {

	void BloomEffect::Create()
	{
		// RENDER TARGET
		{
			JSH_RENDER_TARGET_VIEW_DESC desc;
			desc.Format = JSH_FORMAT_R8G8B8A8_UNORM;
			desc.Texture2D.MipSlice = 0u;
			desc.ViewDimension = JSH_RTV_DIMENSION_TEXTURE2D;

			JSH_TEXTURE2D_DESC resDesc;
			resDesc.ArraySize = 1u;
			resDesc.BindFlags = JSH_BIND_RENDER_TARGET | JSH_BIND_SHADER_RESOURCE;
			resDesc.CPUAccessFlags = 0u;
			resDesc.Format = JSH_FORMAT_R8G8B8A8_UNORM;
			resDesc.Width = 1080;
			resDesc.Height = 720;
			resDesc.MipLevels = 1u;
			resDesc.MiscFlags = 0u;
			resDesc.SampleDesc.Count = 1u;
			resDesc.SampleDesc.Quality = 0u;
			resDesc.Usage = JSH_USAGE_DEFAULT;

			jshGraphics::CreateRenderTargetView(&desc, &resDesc, &m_RenderTargetView);
		}

		// CONSTANT BUFFER
		{
			jsh::vec4 aux;
			aux.x = 0.8f;
			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = 4 * sizeof(float);
			desc.CPUAccessFlags = 0u;
			desc.MiscFlags = 0u;
			desc.StructureByteStride = 0u;
			desc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA sdata;
			sdata.pSysMem = &aux;
			jshGraphics::CreateBuffer(&desc, &sdata, &m_BloomBuffer);
		}

		m_BlurEffect.Create();
	}

	void BloomEffect::Render(RenderTargetView& input, float intensity, uint32 radius, float sigma, CommandList cmd)
	{
		m_BlurEffect.SetAlphaGaussianMode(radius, sigma);

		jshGraphics::ClearRenderTargetView(m_RenderTargetView, 0.f, 0.f, 0.f, 0.f, cmd);
		jshGraphics::BindBlendState(jshGraphics::primitives::GetDefaultBlendState(), cmd);
		jshGraphics::BindViewport(jshGraphics::primitives::GetDefaultViewport(), 0u, cmd);

		jshGraphics::BindConstantBuffer(m_BloomBuffer, 0u, JSH_SHADER_TYPE_PIXEL, cmd);
		
		jsh::vec4 i;
		i.x = intensity;
		jshGraphics::UpdateBuffer(m_BloomBuffer, &i, 0, cmd);

		PixelShader* ps = reinterpret_cast<PixelShader*>(jshGraphics::Get("BloomEffectPixel"));
		jshGraphics::PostProcess(input, m_RenderTargetView, nullptr, nullptr, 0u, ps, cmd);

		jshGraphics::BindBlendState(jshGraphics::primitives::GetTransparentBlendState(), cmd);

		jshGraphics::UnbindTexture(0u, JSH_SHADER_TYPE_PIXEL, cmd);
		m_BlurEffect.Render(m_RenderTargetView, input, nullptr, nullptr, 0u, cmd);
	}

}
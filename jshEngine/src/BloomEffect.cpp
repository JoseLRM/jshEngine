#include "BloomEffect.h"

#include "Renderer.h"
#include "PostProcess.h"
#include "EventSystem.h"

namespace jsh {

	void BloomEffect::Create(uvec2 resolution)
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
			resDesc.Width = resolution.x;
			resDesc.Height = resolution.y;
			resDesc.MipLevels = 1u;
			resDesc.MiscFlags = 0u;
			resDesc.SampleDesc.Count = 1u;
			resDesc.SampleDesc.Quality = 0u;
			resDesc.Usage = JSH_USAGE_DEFAULT;

			jshGraphics::CreateRenderTargetView(&desc, &resDesc, &m_RenderTargetView);
			jshGraphics::CreateViewport(0.f, 0.f, (float)resolution.x, (float)resolution.y, &m_Viewport);
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

		m_BlurEffect.Create(resolution);
	}

	void BloomEffect::SetResolution(uint32 width, uint32 height)
	{
		m_BlurEffect.SetResolution(width, height);
		jshGraphics::ResizeRenderTargetView(m_RenderTargetView, width, height);
		jshGraphics::CreateViewport(0.f, 0.f, (float)width, (float)height, &m_Viewport);
	}

	void BloomEffect::Render(RenderTargetView input, float intensity, uint32 radius, float sigma, CommandList cmd)
	{
		m_BlurEffect.SetAlphaGaussianMode(radius, sigma);

		jshGraphics::ClearRenderTargetView(m_RenderTargetView, 0.f, 0.f, 0.f, 0.f, cmd);
		jshGraphics::BindBlendState(jshGraphics::primitives::GetDefaultBlendState(), cmd);
		jshGraphics::BindViewport(m_Viewport, 0u, cmd);

		jshGraphics::BindConstantBuffers(&m_BloomBuffer, 0u, 1u, JSH_SHADER_TYPE_PIXEL, cmd);
		
		jsh::vec4 i;
		i.x = intensity;
		jshGraphics::UpdateBuffer(m_BloomBuffer, &i, 0, cmd);

		PixelShader* ps = reinterpret_cast<PixelShader*>(jshGraphics::Get("BloomEffectPixel"));
		jshGraphics::PostProcess(input, m_RenderTargetView, nullptr, nullptr, 0u, ps, cmd);

		jshGraphics::BindBlendState(jshGraphics::primitives::GetTransparentBlendState(), cmd);

		jshGraphics::UnbindTexture(0u, JSH_SHADER_TYPE_PIXEL, cmd);
		m_BlurEffect.Render(m_RenderTargetView, input, m_Viewport, nullptr, nullptr, 0u, cmd);
	}

}
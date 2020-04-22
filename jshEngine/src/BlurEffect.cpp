#include "BlurEffect.h"
#include "Renderer.h"
#include "PostProcess.h"
#include "Debug.h"

namespace jsh {

	BlurEffect::BlurEffect() {}

	void BlurEffect::Create()
	{
		// create aux render target view
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

			jshGraphics::CreateRenderTargetView(&desc, &resDesc, &m_AuxRTV);
		}

		// gaussian buffer
		{
			BlurData aux;
			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(BlurData);
			desc.CPUAccessFlags = 0u;
			desc.MiscFlags = 0u;
			desc.StructureByteStride = 0u;
			desc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA sdata;
			sdata.pSysMem = &aux;
			jshGraphics::CreateResource(&desc, &sdata, &m_BlurBuffer);
		}
		// coefficients buffer
		{
			CoefficientsData aux;
			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(CoefficientsData);
			desc.CPUAccessFlags = 0u;
			desc.MiscFlags = 0u;
			desc.StructureByteStride = 0u;
			desc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA sdata;
			sdata.pSysMem = &aux;
			jshGraphics::CreateResource(&desc, &sdata, &m_CoefficientsBuffer);
		}
		// sampler state
		{
			JSH_SAMPLER_DESC desc;
			jshZeroMemory(&desc, sizeof(JSH_SAMPLER_DESC));
			desc.AddressU = JSH_TEXTURE_ADDRESS_MIRROR;
			desc.AddressV = JSH_TEXTURE_ADDRESS_MIRROR;
			desc.AddressW = JSH_TEXTURE_ADDRESS_MIRROR;
			desc.Filter = JSH_FILTER_MIN_MAG_MIP_LINEAR;
			jshGraphics::CreateSamplerState(&desc, &m_SamplerState);
		}
		//blend state
		{
			JSH_BLEND_DESC desc;
			jshZeroMemory(&desc, sizeof(JSH_BLEND_DESC));
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].BlendOp = JSH_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlend = JSH_BLEND_ONE;
			desc.RenderTarget[0].DestBlend = JSH_BLEND_ZERO;

			desc.RenderTarget[0].BlendOpAlpha = JSH_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = JSH_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = JSH_BLEND_ZERO;

			desc.RenderTarget[0].RenderTargetWriteMask = JSH_COLOR_WRITE_ENABLE_ALL;
			jshGraphics::CreateBlendState(&desc, &m_BlendStateAlpha);
		}
	}

	void BlurEffect::Render(RenderTargetView& input, RenderTargetView& output, DepthStencilState* dss, Resource* dsv, uint32 stencilRef, CommandList cmd)
	{
		assert(input.IsValid() && output.IsValid());

		PixelShader* blurPS = nullptr;

		bool alphaMode = m_BlurMode == 1 || m_BlurMode == 3 || m_BlurMode == 4;
		
		if (alphaMode) {
			blurPS = (PixelShader*)jshGraphics::Get("AlphaBlurPixel");
			jshGraphics::BindBlendState(m_BlendStateAlpha, cmd);
		}
		else {
			blurPS = (PixelShader*)jshGraphics::Get("BlurPixel");
			jshGraphics::BindBlendState(jshRenderer::primitives::GetDefaultBlendState(), cmd);
		}

		jshGraphics::ClearRenderTargetView(m_AuxRTV, 0.f, 0.f, 0.f, 0.f, cmd);

		if (m_Radius > 16) m_Radius = 16;

		BlurData blurData;
		blurData.count = m_Radius;
		blurData.horizontal = true;
		blurData.offset = 1.f / 1080.f;

		CoefficientsData coefficientsData;
		if (m_BlurMode == 0 || m_BlurMode == 1) LoadCoefficientsGaussianMode(coefficientsData);
		else if (m_BlurMode == 2 || m_BlurMode == 3) LoadCoefficientsBoxMode(coefficientsData);
		else LoadCoefficientsSolidMode(coefficientsData);

		jshGraphics::UpdateConstantBuffer(m_BlurBuffer, &blurData, cmd);
		jshGraphics::UpdateConstantBuffer(m_CoefficientsBuffer, &coefficientsData, cmd);
		jshGraphics::BindConstantBuffer(m_BlurBuffer, 0u, JSH_SHADER_TYPE_PIXEL, cmd);
		jshGraphics::BindConstantBuffer(m_CoefficientsBuffer, 1u, JSH_SHADER_TYPE_PIXEL, cmd);

		jshGraphics::BindSamplerState(m_SamplerState, 0u, JSH_SHADER_TYPE_PIXEL, cmd);
		
		jshRenderer::PostProcess(input, m_AuxRTV, nullptr, nullptr, 0u, blurPS, cmd);		

		blurData.offset = 1.f / 720.f;
		blurData.horizontal = false;
		jshGraphics::UpdateConstantBuffer(m_BlurBuffer, &blurData, cmd);

		if(alphaMode) jshGraphics::BindBlendState(jshRenderer::primitives::GetTransparentBlendState(), cmd);

		jshRenderer::PostProcess(m_AuxRTV, output, dss, dsv, stencilRef, blurPS, cmd);

	}

	void BlurEffect::LoadCoefficientsSolidMode(CoefficientsData& c) const noexcept
	{
		for (uint32 i = 0; i < m_Radius; ++i) {
			c.coefficients[i].x = 1.f;
		}
	}
	void BlurEffect::LoadCoefficientsBoxMode(CoefficientsData& c) const noexcept
	{
		float value = 0.5f / m_Radius;
		for (uint32 i = 0; i < m_Radius; ++i) {
			c.coefficients[i].x = value;
		}
	}
	void BlurEffect::LoadCoefficientsGaussianMode(CoefficientsData& c) const noexcept
	{
		float co = 0.f;

		for (uint32 x = 0; x < m_Radius; ++x) {
			c.coefficients[x].x = jsh::Gauss((float)x, m_Sigma);
			co += c.coefficients[x].x;
		}

		float realCo = (co / (float)m_Radius) * ((float)m_Radius * 2 - 1);

		for (uint32 i = 0; i < m_Radius; ++i) {
			c.coefficients[i].x /= realCo;
		}
	}

}
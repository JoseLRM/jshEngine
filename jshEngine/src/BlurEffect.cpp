#include "BlurEffect.h"
#include "Renderer.h"
#include "PostProcess.h"

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
			GaussianData aux;
			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(GaussianData);
			desc.CPUAccessFlags = 0u;
			desc.MiscFlags = 0u;
			desc.StructureByteStride = 0u;
			desc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA sdata;
			sdata.pSysMem = &aux;
			jshGraphics::CreateResource(&desc, &sdata, &m_GaussianBuffer);
		}
	}

	void BlurEffect::Render(RenderTargetView& input, RenderTargetView& output, DepthStencilState* dss, Resource* dsv, uint32 stencilRef, CommandList cmd)
	{
		assert(input.IsValid() && output.IsValid());

		PixelShader* gaussianBlur = (PixelShader*)jshGraphics::Get("GaussianBlurPixel");

		jshGraphics::ClearRenderTargetView(m_AuxRTV, cmd);

		GaussianData gaussianData;
		gaussianData.count = 16;
		gaussianData.horizontal = true;
		gaussianData.offset = 1.f / 720.f;
		for(uint32 i = 0; i < 17; ++i) gaussianData.coefficients[i] = 0.25f;

		jshGraphics::UpdateConstantBuffer(m_GaussianBuffer, &gaussianData, cmd);
		jshGraphics::BindConstantBuffer(m_GaussianBuffer, 0u, JSH_SHADER_TYPE_PIXEL, cmd);

		jshGraphics::PostProcess(input, m_AuxRTV, nullptr, nullptr, 0u, gaussianBlur, cmd);

		gaussianData.offset = 1.f / 1080.f;
		gaussianData.horizontal = false;
		jshGraphics::UpdateConstantBuffer(m_GaussianBuffer, &gaussianData, cmd);
		
		jshGraphics::PostProcess(m_AuxRTV, output, dss, dsv, stencilRef, gaussianBlur, cmd);

	}

}
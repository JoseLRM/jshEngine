#include "PostProcess.h"

#include "Renderer.h"

using namespace jsh;

namespace jshGraphics {

	// static
	Buffer g_VertexBuffer;
	VertexShader g_VertexShader;
	PixelShader g_DefaultPixelShader;
	InputLayout g_InputLayout;

	void InitializePostProcess()
	{
		float vData[] = {
				-1.f, 1.f,
				 1.f, 1.f,
				-1.f,-1.f,
				 1.f,-1.f
		};

		JSH_BUFFER_DESC vDesc;
		vDesc.BindFlags = JSH_BIND_VERTEX_BUFFER;
		vDesc.ByteWidth = sizeof(float) * 8;
		vDesc.CPUAccessFlags = 0u;
		vDesc.MiscFlags = 0u;
		vDesc.StructureByteStride = 2 * sizeof(float);
		vDesc.Usage = JSH_USAGE_DEFAULT;
		JSH_SUBRESOURCE_DATA vSubData;
		vSubData.pSysMem = &vData;
		jshGraphics::CreateBuffer(&vDesc, &vSubData, &g_VertexBuffer);

		const JSH_INPUT_ELEMENT_DESC ilDesc[] = {
			{"Position", 0, JSH_FORMAT_R32G32_FLOAT, 0, true, 0, 0}
		};

		jshGraphics::CreateVertexShader(L"PostProcessVertex.cso", &g_VertexShader);
		jshGraphics::CreatePixelShader(L"PP.cso", &g_DefaultPixelShader);
		jshGraphics::CreateInputLayout(ilDesc, 1, g_VertexShader, &g_InputLayout);

		std::shared_ptr<PixelShader> gaussianBlur = std::make_shared<PixelShader>();
		jshGraphics::Save("BlurPixel", gaussianBlur);
		jshGraphics::CreatePixelShader(L"PPBlur.cso", gaussianBlur.get());

		std::shared_ptr<PixelShader> alphaGaussianBlur = std::make_shared<PixelShader>();
		jshGraphics::Save("AlphaBlurPixel", alphaGaussianBlur);
		jshGraphics::CreatePixelShader(L"PPAlphaBlur.cso", alphaGaussianBlur.get());

		std::shared_ptr<PixelShader> bloomEffect = std::make_shared<PixelShader>();
		jshGraphics::Save("BloomEffectPixel", bloomEffect);
		jshGraphics::CreatePixelShader(L"PPBloom.cso", bloomEffect.get());

		std::shared_ptr<PixelShader> colorCorrection = std::make_shared<PixelShader>();
		jshGraphics::Save("ColorCorrectionPixel", colorCorrection);
		jshGraphics::CreatePixelShader(L"PPColorCorrection.cso", colorCorrection.get());
	}

	void PostProcess(const jsh::RenderTargetView& input, const jsh::RenderTargetView& output, jsh::DepthStencilState* dss, jsh::TextureRes* dsv, uint32 stencilRef, jsh::PixelShader* ps, jsh::CommandList cmd)
	{
		assert(input.IsValid() && output.IsValid());

		if (dss && dsv) {
			jshGraphics::BindDepthStencilState(*dss, stencilRef, cmd);
			jshGraphics::BindRenderTargetView(output, *dsv, cmd);
		}
		else jshGraphics::BindRenderTargetView(output, cmd);

		jshGraphics::BindTexture(input, 0u, JSH_SHADER_TYPE_PIXEL, cmd);

		jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLE_STRIP, cmd);

		jshGraphics::BindVertexBuffer(g_VertexBuffer, 0u, cmd);
		jshGraphics::BindInputLayout(g_InputLayout, cmd);
		jshGraphics::BindVertexShader(g_VertexShader, cmd);
		jshGraphics::BindPixelShader((ps == nullptr) ? g_DefaultPixelShader : *ps, cmd);

		jshGraphics::Draw(4, cmd);
	}

}
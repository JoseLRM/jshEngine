#include "PostProcessRenderPass.h"

#include "Renderer.h"
#include "Components.h"
#include "PostProcess.h"

namespace jsh {

	void PostProcessRenderPass::Create()
	{
		m_BloomEffect.Create();

		{
			jsh::vec4 aux;
			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(float) * 4;
			desc.CPUAccessFlags = 0u;
			desc.MiscFlags = 0u;
			desc.StructureByteStride = 0u;
			desc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA sdata;
			sdata.pSysMem = &aux;
			jshGraphics::CreateBuffer(&desc, &sdata, &m_ColorCorrectionBuffer);
		}
	}

	void PostProcessRenderPass::Load()
	{

	}

	void PostProcessRenderPass::Render(CommandList cmd)
	{
		RenderTargetView& offscreenRTV = jshGraphics::primitives::GetOffscreenRenderTargetView();
		RenderTargetView& mainRTV = jshGraphics::primitives::GetMainRenderTargetView();

		jshGraphics::BindViewport(jshGraphics::primitives::GetDefaultViewport(), 0u, cmd);
		jshGraphics::BindSamplerState(jshGraphics::primitives::GetDefaultSamplerState(), 0u, JSH_SHADER_TYPE_PIXEL, cmd);

		CameraComponent* camera = m_pRenderGraph->GetCurrentCamera();
		PostProcessComponent* PP = jshScene::GetComponent<PostProcessComponent>(camera->entity);

		if (PP) {

			if (PP->IsBloomEffect()) {
				m_BloomEffect.Render(offscreenRTV, PP->GetBloomIntensity(), PP->GetBloomRadius(), PP->GetBloomSigma(), cmd);
			}

			jsh::vec4 ccData;
			ccData.x = PP->GetContrast();
			ccData.y = PP->GetBrightness();

			jshGraphics::UpdateBuffer(m_ColorCorrectionBuffer, &ccData, 0u, cmd);
			jshGraphics::BindConstantBuffer(m_ColorCorrectionBuffer, 0u, JSH_SHADER_TYPE_PIXEL, cmd);

			jshGraphics::PostProcess(offscreenRTV, mainRTV, nullptr, nullptr, 0u, reinterpret_cast<PixelShader*>(jshGraphics::Get("ColorCorrectionPixel")), cmd);
		}
		else {
			jshGraphics::PostProcess(offscreenRTV, mainRTV, nullptr, nullptr, 0u, nullptr, cmd);
		}
		
	}

}
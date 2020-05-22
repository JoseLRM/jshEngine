#include "common.h"

#include "PostProcessTechnique.h"

#include "Components.h"
#include "PostProcess.h"

namespace jsh {

	PostProcessTechnique::PostProcessTechnique(RenderTargetView& rtv, RenderTargetView& auxRtv, Viewport& vp)
	{
		AddAttachment(reinterpret_cast<GfxPrimitive*>(&rtv));
		AddAttachment(reinterpret_cast<GfxPrimitive*>(&auxRtv));
		m_Viewport = &vp;
	}
	
	void PostProcessTechnique::Initialize()
	{
		//m_BloomEffect.Create(jshGraphics::GetResolution());

		//jshEvent::Register<OutputModeEvent>(JSH_EVENT_LAYER_SYSTEM, [this](OutputModeEvent& e) {
		//	m_BloomEffect.SetResolution(e.outputMode.resolution.x, e.outputMode.resolution.y);
		//	return true;
		//});

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

	void PostProcessTechnique::Update()
	{

	}

	void PostProcessTechnique::Render(CommandList cmd)
	{
		RenderTargetView& offscreenRTV = *reinterpret_cast<RenderTargetView*>(GetAttachment(0));
		RenderTargetView& auxRTV = *reinterpret_cast<RenderTargetView*>(GetAttachment(1));

		jshGraphics::BindViewport(*m_Viewport, 0u, cmd);
		jshGraphics::BindSamplerState(jshGraphics::primitives::GetDefaultSamplerState(), 0u, JSH_SHADER_TYPE_PIXEL, cmd);

		Camera* camera = GetRenderGraph().GetCamera();
		PostProcessComponent* PP = jshScene::GetComponent<PostProcessComponent>(jshRenderer::GetCamera()->entity);

		if (PP) {

			if (PP->IsBloomEffect()) {
				//m_BloomEffect.Render(offscreenRTV, PP->GetBloomIntensity(), PP->GetBloomRadius(), PP->GetBloomSigma(), cmd);
			}

			jsh::vec4 ccData;
			ccData.x = PP->GetContrast();
			ccData.y = PP->GetBrightness();

			jshGraphics::UpdateBuffer(m_ColorCorrectionBuffer, &ccData, 0u, cmd);
			const Buffer* ccBuffer = &m_ColorCorrectionBuffer;
			jshGraphics::BindConstantBuffers(&ccBuffer, 0u, 1u, JSH_SHADER_TYPE_PIXEL, cmd);

			jshGraphics::PostProcess(offscreenRTV, auxRTV, nullptr, nullptr, 0u, reinterpret_cast<PixelShader*>(jshGraphics::Get("ColorCorrectionPixel")), cmd);
			jshGraphics::UnbindTexture(0, JSH_SHADER_TYPE_PIXEL, cmd);
			jshGraphics::PostProcess(auxRTV, offscreenRTV, nullptr, nullptr, 0u, nullptr, cmd);
		}
		
	}

	void PostProcessTechnique::Close()
	{

	}

}
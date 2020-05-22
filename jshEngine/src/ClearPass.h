#pragma once

#include "Technique.h"

namespace jsh {

	class ClearPass : public jsh::Technique {
		/*
		1 -> RenderTargetView
		2 -> DepthStencilView
		*/
		uint8 m_AttachmentsTypes[JSH_GFX_ATTACHMENTS_COUNT];
		vec4 m_ClearColors[JSH_GFX_ATTACHMENTS_COUNT];
		uint8 m_NumOfAttachments = 0u;

	public:
		void AddRenderTargetView(RenderTargetView& rtv, float r, float g, float b, float a);
		void AddDepthStencilView(TextureRes& dsv);

	protected:
		void Initialize() override;
		void Update() override;
		void Render(CommandList cmd) override;
		void Close() override;

	};

}
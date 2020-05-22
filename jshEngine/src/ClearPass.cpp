#include "common.h"

#include "ClearPass.h"

namespace jsh {

	void ClearPass::AddRenderTargetView(RenderTargetView& rtv, float r, float g, float b, float a)
	{
		uint8 i = AddAttachment(reinterpret_cast<GfxPrimitive*>(&rtv));
		m_AttachmentsTypes[i] = 1;
		m_ClearColors[i] = { r, g, b, a };
		m_NumOfAttachments++;
	}
	void ClearPass::AddDepthStencilView(TextureRes& dsv)
	{
		uint8 i = AddAttachment(reinterpret_cast<GfxPrimitive*>(&dsv));
		m_AttachmentsTypes[i] = 2;
		m_NumOfAttachments++;
	}

	void ClearPass::Initialize()
	{
	}
	void ClearPass::Update()
	{
	}
	void ClearPass::Render(CommandList cmd)
	{
		for (uint32 i = 0; i < m_NumOfAttachments; ++i) {
			if (m_AttachmentsTypes[i] == 1u) {
				RenderTargetView* rtv = reinterpret_cast<RenderTargetView*>(GetAttachment(i));
				vec4& clearColor = m_ClearColors[i];
				jshGraphics::ClearRenderTargetView(*rtv, clearColor.x, clearColor.y, clearColor.z, clearColor.w, cmd);
			}
			else {
				TextureRes* dsv = reinterpret_cast<TextureRes*>(GetAttachment(i));
				vec4& clearColor = m_ClearColors[i];
				jshGraphics::ClearDepthStencilView(*dsv, cmd);
			}
		}
	}
	void ClearPass::Close()
	{
	}

}
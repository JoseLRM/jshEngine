#pragma once

#include "Renderer.h"

#include "DrawRenderPass.h"
#include "OutlineRenderPass.h"
#include "PostProcessRenderPass.h"

namespace jsh {

	class Renderer3D : public Renderer {
		DrawRenderPass		m_Rendering3DPass;
		//OutlineRenderPass		m_OutlineRenderPass;
		PostProcessRenderPass	m_PostProcessRenderPass;

		RenderQueue m_RenderQueue;
		SpriteRenderQueue m_SpriteRenderQueue;

	public:
		bool Initialize() override;

		void Begin() override;
		void Render() override;
		void End() override;
		
		bool Close() override;

	};

}
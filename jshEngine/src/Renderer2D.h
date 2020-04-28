#pragma once

#include "Renderer.h"
#include "Rendering2DPass.h"
#include "PostProcessRenderPass.h"
#include "OutlineRenderPass.h"
#include "SpriteBatch.h"

namespace jsh {

	class Renderer2D : public Renderer {
		Rendering2DPass m_Rendering2DPass;
		OutlineRenderPass m_OutlineRenderPass;
		PostProcessRenderPass m_PostProcessRenderPass;

		SpriteBatch m_SpriteBatch;

	public:
		bool Initialize() override;
		void Begin() override;
		void Render() override;
		void End() override;
		bool Close() override;

	};

}
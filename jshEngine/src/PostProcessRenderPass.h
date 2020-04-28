#pragma once

#include "RenderGraph.h"
#include "BloomEffect.h"

namespace jsh {

	class PostProcessRenderPass : public RenderPass {

		BloomEffect m_BloomEffect;
		Buffer m_ColorCorrectionBuffer;

	public:
		void Create() override;
		void Load() override;
		void Render(CommandList cmd) override;

	};

}
#pragma once

#include "Technique.h"

namespace jsh {

	class PostProcessTechnique : public Technique {
		
		Buffer m_ColorCorrectionBuffer;
		Viewport* m_Viewport;

	public:
		PostProcessTechnique(RenderTargetView& rtv, RenderTargetView& auxRtv, Viewport& vp);

		void Initialize() override;
		void Update() override;
		void Render(CommandList cmd) override;
		void Close() override;

	};

}
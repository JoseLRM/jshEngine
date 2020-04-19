#pragma once

#include "..//Graphics.h"

namespace jsh {

	class BlurEffect {

		RenderTargetView  m_AuxRTV;
		Resource m_GaussianBuffer;

		uint8 m_BlurMode = 0;
		/*
			Blur mode 0 = Gaussian Blur
		*/

		struct alignas(16) GaussianData {
			float offset;
			int32 count;
			BOOL horizontal;
			float coefficients[17];
		};

	public:
		BlurEffect();

		void Create();
		void Render(RenderTargetView& input, RenderTargetView& output, DepthStencilState* dss, Resource* dsv, uint32 stencilRef, CommandList cmd);

	};

}
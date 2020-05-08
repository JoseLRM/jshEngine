#pragma once

#include "BlurEffect.h"

namespace jsh {

	class BloomEffect {

		BlurEffect m_BlurEffect;
		RenderTargetView m_RenderTargetView;
		Viewport m_Viewport;
		Buffer m_BloomBuffer;

	public:
		void Create(uvec2 resolution);
		void SetResolution(uint32 width, uint32 height);
		void Render(RenderTargetView input, float intensity, uint32 radius, float sigma, CommandList cmd);

	};

}
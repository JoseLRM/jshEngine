#pragma once

#include "BlurEffect.h"

namespace jsh {

	class BloomEffect {

		BlurEffect m_BlurEffect;
		RenderTargetView m_RenderTargetView;
		Buffer m_BloomBuffer;

	public:
		void Create();
		void Render(RenderTargetView& input, float intensity, uint32 radius, float sigma, CommandList cmd);

	};

}
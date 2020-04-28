#pragma once

#include "Graphics.h"

namespace jshGraphics {

#ifdef JSH_ENGINE
	void InitializePostProcess();
#endif

	void PostProcess(const jsh::RenderTargetView& input, const jsh::RenderTargetView& output, jsh::DepthStencilState* dss, jsh::TextureRes* dsv, uint32 stencilRef, jsh::PixelShader* ps, jsh::CommandList cmd);

}
#pragma once

#include "GraphicsPrimitives.h"
#include "GraphicsObjects.h"

namespace jshGraphics {

#ifdef JSH_ENGINE
	bool Initialize();
	bool Close();

#ifdef JSH_IMGUI
	bool InitializeImGui();
#endif
#endif

	JSH_GRAPHICS_API GetAPI();

	void Begin();
	void End();
	void Present(uint32 interval);

#ifdef JSH_IMGUI
	void BeginImGui();
	void EndImGui(const jsh::RenderTargetView& rtv);
#endif

	void SetResolution(uint32 width, uint32 height);
	jsh::uvec2 GetResolution();
	void SetFullscreen(bool fullscreen);
	bool InFullscreen();

}
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
	
	void SetTopology(JSH_TOPOLOGY topology, jsh::CommandList cmd);

	void Begin();
	void End();
	void Present();

#ifdef JSH_IMGUI
	void BeginImGui();
	void EndImGui(const jsh::RenderTargetView& rtv);
#endif

	jsh::CommandList BeginCommandList();

	void UpdateConstantBuffer(jsh::Buffer buffer, void* data, jsh::CommandList cmd);
	//////////////////DRAW CALLS//////////////////
	void DrawIndexed(uint32 indicesCount, jsh::CommandList cmd);

}
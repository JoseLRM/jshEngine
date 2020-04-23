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
	void Present(uint32 interval);

#ifdef JSH_IMGUI
	void BeginImGui();
	void EndImGui(const jsh::RenderTargetView& rtv);
#endif

	jsh::CommandList BeginCommandList();

	void UpdateConstantBuffer(jsh::Resource buffer, void* data, jsh::CommandList cmd);
	//////////////////DRAW CALLS//////////////////
	void Draw(uint32 vertexCount, jsh::CommandList cmd);
	void DrawIndexed(uint32 indicesCount, jsh::CommandList cmd);

}
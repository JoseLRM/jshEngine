#pragma once

#include "GraphicsPrimitives.h"
#include "GraphicsObjects.h"

namespace jshGraphics {

#ifdef JSH_ENGINE
	bool Initialize();

#ifdef JSH_IMGUI
	bool InitializeImGui();
#endif

	bool Close();
	void Prepare();

#endif

	JSH_GRAPHICS_API GetAPI();

	void SetClearScreenColor(float c = 0.f);
	void SetClearScreenColor(float r, float g, float b);

	void SetTopology(JSH_TOPOLOGY topology);

}


// Abstracted methods
namespace jshGraphics {
	
	void UpdateConstantBuffer(jsh::Buffer buffer, void* data);

	//////////////////DRAW CALLS//////////////////
	void DrawIndexed(uint32 indicesCount);

}
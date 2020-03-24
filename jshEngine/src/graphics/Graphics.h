#pragma once

#include "GraphicsDesc.h"

namespace jshGraphics {

#ifdef JSH_ENGINE
	bool Initialize();

#ifdef JSH_IMGUI
	bool InitializeImGui();
#endif

	bool Close();
	void Prepare();

#endif


	void SetClearScreenColor(float c = 0.f);
	void SetClearScreenColor(float r, float g, float b);

	void SetTopology(JSH_TOPOLOGY topology);

}

namespace jshGraphics {

	/////////////////////CREATE///////////////////////////
	jsh::Buffer CreateBuffer(void* data, uint32 size, uint32 stride, JSH_USAGE usage, JSH_BUFFER_TYPE bufferType);
	
	jsh::InputLayout CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* descriptors, uint32 cant, jsh::VertexShader);

	jsh::VertexShader CreateVertexShader(const wchar* path);
	jsh::VertexShader CreatePixelShader(const wchar* path);

	////////////////////BIND///////////////////////////////
	
	void BindVertexBuffer(jsh::Buffer buffer, uint32 slot);
	void BindIndexBuffer(jsh::Buffer buffer, uint32 slot);
	void BindConstantBuffer(jsh::Buffer buffer, uint32 slot, JSH_SHADER_TYPE shaderType);

	void BindInputLayout(jsh::InputLayout inputLayout);

	void BindVertexShader(jsh::VertexShader);
	void BindPixelShader(jsh::PixelShader);

	//////////////////DRAW CALLS//////////////////
	void DrawIndexed(uint32 indicesCount);

}
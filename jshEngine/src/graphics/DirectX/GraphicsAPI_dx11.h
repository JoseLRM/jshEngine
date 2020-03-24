#pragma once

#include "..//GraphicsDesc.h"


namespace jshGraphics_dx11 {

	////////////////////FUNCTIONS///////////////////////////

	bool Initialize();

	jshImGui(bool InitializeImGui());

	bool Close();

	void Prepare(float* clearScreenColor);
	void SetTopology(JSH_TOPOLOGY);

	//////////////////////////////CREATION//////////////////////////////
	jsh::Buffer CreateBuffer(void* data, uint32 size, uint32 stride, JSH_USAGE usage, JSH_BUFFER_TYPE bufferType);
	
	jsh::InputLayout CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* descriptors, uint32 cant, jsh::VertexShader);
	
	jsh::VertexShader CreateVertexShader(const wchar* path);
	jsh::VertexShader CreatePixelShader(const wchar* path);

	/////////////////////////BINDING////////////////////////////////////////
	void BindVertexBuffer(jsh::Buffer ID, uint32 slot);
	void BindIndexBuffer(jsh::Buffer ID, uint32 slot);
	void BindConstantBuffer(jsh::Buffer ID, uint32 slot, JSH_SHADER_TYPE shaderType);

	void BindInputLayout(jsh::InputLayout inputLayout);

	void BindVertexShader(jsh::VertexShader);
	void BindPixelShader(jsh::PixelShader);

	//////////////////////////RENDER CALLS////////////////////////////////////
	void DrawIndexed(uint32 indicesCount);

}
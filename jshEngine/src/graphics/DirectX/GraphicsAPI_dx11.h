#pragma once

#include "..//GraphicsPrimitives.h"


namespace jshGraphics_dx11 {

	////////////////////FUNCTIONS///////////////////////////

	bool Initialize();

	jshImGui(bool InitializeImGui());

	bool Close();

	void Prepare(float* clearScreenColor);
	void SetTopology(JSH_TOPOLOGY);

	//////////////////////////////CREATION//////////////////////////////
	jsh::Buffer CreateBuffer(void* data, uint32 size, uint32 stride, JSH_USAGE usage, uint32 slot, JSH_BUFFER_TYPE bufferType, JSH_SHADER_TYPE constShaderType = JSH_SHADER_TYPE_VERTEX);

	jsh::InputLayout CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* descriptors, uint32 cant, jsh::VertexShader);
	
	jsh::Texture CreateTexture(void* data, uint32 pitch, uint32 width, uint32 height, uint32 slot, JSH_FORMAT format, JSH_SHADER_TYPE shaderType, jsh::Sampler sampler);

	jsh::VertexShader CreateVertexShader(const wchar* path);
	jsh::VertexShader CreatePixelShader(const wchar* path);

	jsh::DepthStencilState CreateDepthStencilState(bool depth, bool stencil);

	jsh::Sampler CreateSampler(JSH_FILTER filter, JSH_TEXTURE_ADDRESS_MODE addressMode, uint32 slot, JSH_SHADER_TYPE shaderType);

	/////////////////////////BINDING////////////////////////////////////////
	void BindVertexBuffer(jsh::Buffer buffer);
	void BindIndexBuffer(jsh::Buffer buffer);
	void BindConstantBuffer(jsh::Buffer buffer);
	void UpdateConstantBuffer(jsh::Buffer buffer, void* data);

	void BindInputLayout(jsh::InputLayout inputLayout);

	void BindTexture(jsh::Texture texture);

	void BindVertexShader(jsh::VertexShader);
	void BindPixelShader(jsh::PixelShader);

	void BindDepthStencilState(jsh::DepthStencilState state);

	void BindSampler(jsh::Sampler sampler);

	//////////////////////////RENDER CALLS////////////////////////////////////
	void DrawIndexed(uint32 indicesCount);

}
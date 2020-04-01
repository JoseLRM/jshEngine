#pragma once

#include "..//GraphicsPrimitives.h"


namespace jshGraphics_dx11 {

	////////////////////FUNCTIONS///////////////////////////

	bool Initialize();

	jshImGui(bool InitializeImGui());

	bool Close();

	void Prepare(float* clearScreenColor);
	void SetTopology(JSH_TOPOLOGY);

	//////////////////////////////BUFFER//////////////////////////////
	jsh::Buffer CreateBuffer(void* data, uint32 size, uint32 stride, JSH_USAGE usage, JSH_BUFFER_TYPE bufferType);

	void BindVertexBuffer(jsh::Buffer buffer, uint32 slot);
	void BindIndexBuffer(jsh::Buffer buffer);
	void BindConstantBuffer(jsh::Buffer buffer, uint32 slot, JSH_SHADER_TYPE shaderType);

	//////////////////////////////INPUT LAYOUT//////////////////////////////
	jsh::InputLayout CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* descriptors, uint32 cant, jsh::VertexShader);
	void BindInputLayout(jsh::InputLayout inputLayout);

	//////////////////////////////TEXTURE//////////////////////////////
	jsh::Texture CreateTexture(void* data, uint32 pitch, uint32 width, uint32 height, JSH_FORMAT format);
	void SetSamplerState(jsh::Texture texture, JSH_FILTER filter, JSH_TEXTURE_ADDRESS_MODE addressMode);
	void BindTexture(jsh::Texture texture, uint32 slot, JSH_SHADER_TYPE shaderType);

	//////////////////////////////SHADER//////////////////////////////
	jsh::VertexShader CreateVertexShader(const wchar* path);
	jsh::VertexShader CreatePixelShader(const wchar* path);
	void BindVertexShader(jsh::VertexShader);
	void BindPixelShader(jsh::PixelShader);

	//////////////////////////////FRAME BUFFER//////////////////////////////
	jsh::FrameBuffer CreateFrameBuffer(uint32 width, uint32 height);
	void BindFrameBuffer(jsh::FrameBuffer fb);

	void SetDepthState(bool enable, jsh::FrameBuffer fb);
	void SetStencilState(bool enable, jsh::FrameBuffer fb);

	/////////////////////////METHODS////////////////////////////////////////
	void UpdateConstantBuffer(jsh::Buffer buffer, void* data);

	//////////////////////////RENDER CALLS////////////////////////////////////
	void DrawIndexed(uint32 indicesCount);

}
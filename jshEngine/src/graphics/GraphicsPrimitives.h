#pragma once

#include "GraphicsDesc.h"

namespace jsh {

	typedef uint16 Buffer;
	typedef uint16 InputLayout;
	typedef uint16 Texture;

	typedef uint16 VertexShader;
	typedef uint16 PixelShader;

	typedef uint16 Viewport;

	typedef uint16 FrameBuffer;

	constexpr Buffer INVALID_BUFFER = 0u;
	constexpr InputLayout INVALID_INPUT_LAYOUT = 0u;
	constexpr Texture INVALID_TEXTURE = 0u;
	constexpr uint16 INVALID_SHADER = 0u;
	constexpr Viewport INVALID_VIEWPORT = 0u;
	constexpr FrameBuffer INVALID_FRAME_BUFFER = 0u;

	struct Bindable {
		uint16 ID = 0u;
		uint32 param0 = 0u;
		uint8 param1 = 0u;
		JSH_GRAPHICS_PRIMITIVE primitiveType = JSH_GRAPHICS_PRIMITIVE_INVALID;
	};

}

namespace jshGraphics {

	void Bind(const jsh::Bindable& bindable);

	/////////////////////////BUFFER//////////////////////
	jsh::Buffer CreateBuffer(void* data, uint32 size, uint32 stride, JSH_USAGE usage, JSH_BUFFER_TYPE bufferType);
	void BindVertexBuffer(jsh::Buffer buffer, uint32 slot);
	void BindIndexBuffer(jsh::Buffer buffer);
	void BindConstantBuffer(jsh::Buffer buffer, uint32 slot, JSH_SHADER_TYPE constShaderType);


	/////////////////////////INPUTLAYOUT////////////////////////
	jsh::InputLayout CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* descriptors, uint32 cant, jsh::VertexShader);
	void BindInputLayout(jsh::InputLayout inputLayout);

	/////////////////////////SHADER////////////////////////
	jsh::VertexShader CreateVertexShader(const wchar* path);
	jsh::VertexShader CreatePixelShader(const wchar* path);
	void BindVertexShader(jsh::VertexShader);
	void BindPixelShader(jsh::PixelShader);

	/////////////////////////TEXTURE////////////////////////
	jsh::Texture CreateTexture(void* data, uint32 pitch, uint32 width, uint32 height, JSH_FORMAT format);
	void BindTexture(jsh::Texture texture, uint32 slot, JSH_SHADER_TYPE shaderType);

	void SetSamplerState(jsh::Texture texture, JSH_FILTER filter, JSH_TEXTURE_ADDRESS_MODE addressMode);

	/////////////////////////VIEWPORT////////////////////////


	/////////////////////////FRAME BUFFER////////////////////////
	jsh::FrameBuffer CreateFrameBuffer(uint32 width, uint32 height);
	void BindFrameBuffer(jsh::FrameBuffer fb);

	void SetDepthState(bool enable, jsh::FrameBuffer fb = jsh::INVALID_FRAME_BUFFER);
	void SetStencilState(bool enable, jsh::FrameBuffer fb = jsh::INVALID_FRAME_BUFFER);

}
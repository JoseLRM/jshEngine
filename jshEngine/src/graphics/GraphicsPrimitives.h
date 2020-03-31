#pragma once

#include "GraphicsDesc.h"

namespace jsh {

	typedef uint16 Buffer;
	typedef uint16 InputLayout;
	typedef uint16 Texture;

	typedef uint16 VertexShader;
	typedef uint16 PixelShader;

	typedef uint16 Viewport;

	typedef uint16 DepthStencilState;
	typedef uint16 Sampler;

	constexpr Buffer INVALID_BUFFER = 0u;
	constexpr InputLayout INVALID_INPUT_LAYOUT = 0u;
	constexpr Texture INVALID_TEXTURE = 0u;
	constexpr uint16 INVALID_SHADER = 0u;
	constexpr Viewport INVALID_VIEWPORT = 0u;
	constexpr DepthStencilState INVALID_DEPTHSTENCIL_STATE = 0u;
	constexpr Sampler INVALID_SAMPLER = 0u;

	struct Bindable {
		uint16 ID = 0u;
		JSH_GRAPHICS_PRIMITIVE primitiveType = JSH_GRAPHICS_PRIMITIVE_INVALID;
	};

}

namespace jshGraphics {

	void Bind(const jsh::Bindable& bindable);

	/////////////////////////BUFFER//////////////////////
	jsh::Buffer CreateBuffer(void* data, uint32 size, uint32 stride, JSH_USAGE usage, uint32 slot, JSH_BUFFER_TYPE bufferType, JSH_SHADER_TYPE constShaderType = JSH_SHADER_TYPE_VERTEX);
	void BindVertexBuffer(jsh::Buffer buffer);
	void BindIndexBuffer(jsh::Buffer buffer);
	void BindConstantBuffer(jsh::Buffer buffer);


	/////////////////////////INPUTLAYOUT////////////////////////
	jsh::InputLayout CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* descriptors, uint32 cant, jsh::VertexShader);
	void BindInputLayout(jsh::InputLayout inputLayout);

	/////////////////////////SHADER////////////////////////
	jsh::VertexShader CreateVertexShader(const wchar* path);
	jsh::VertexShader CreatePixelShader(const wchar* path);
	void BindVertexShader(jsh::VertexShader);
	void BindPixelShader(jsh::PixelShader);

	/////////////////////////TEXTURE////////////////////////
	jsh::Texture CreateTexture(void* data, uint32 pitch, uint32 width, uint32 height, uint32 slot, JSH_FORMAT format, JSH_SHADER_TYPE shaderType, jsh::Sampler sampler);
	void BindTexture(jsh::Texture texture);

	/////////////////////////VIEWPORT////////////////////////


	/////////////////////////DEPTHSTENCILSTATE////////////////////////
	jsh::DepthStencilState CreateDepthStencilState(bool depth, bool stencil);
	void BindDepthStencilState(jsh::DepthStencilState state);

	/////////////////////////SAMPLER////////////////////////
	jsh::Sampler CreateSampler(JSH_FILTER filter, JSH_TEXTURE_ADDRESS_MODE addressMode, uint32 slot, JSH_SHADER_TYPE shaderType);
	void BindSampler(jsh::Sampler sampler);

}
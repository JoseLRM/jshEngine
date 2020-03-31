#include "GraphicsPrimitives.h"

#include "..//Debug.h"

#include "Graphics.h"
#include "DirectX/GraphicsAPI_dx11.h"

using namespace jsh;

namespace jshGraphics {

	void Bind(const Bindable& bindable)
	{

		switch (bindable.primitiveType)
		{
		case JSH_GRAPHICS_PRIMITIVE_VERTEX_BUFFER:
			BindVertexBuffer(bindable.ID);
			return;
		case JSH_GRAPHICS_PRIMITIVE_INDEX_BUFFER:
			BindIndexBuffer(bindable.ID);
			return;
		case JSH_GRAPHICS_PRIMITIVE_CONSTANT_BUFFER:
			BindConstantBuffer(bindable.ID);
			return;
		case JSH_GRAPHICS_PRIMITIVE_VERTEX_SHADER:
			BindVertexShader(bindable.ID);
			return;
		case JSH_GRAPHICS_PRIMITIVE_PIXEL_SHADER:
			BindPixelShader(bindable.ID);
			return;
		case JSH_GRAPHICS_PRIMITIVE_INPUT_LAYOUT:
			BindInputLayout(bindable.ID);
			return;
		case JSH_GRAPHICS_PRIMITIVE_DEPTHSTENCIL_STATE:
			BindDepthStencilState(bindable.ID);
			return;
		case JSH_GRAPHICS_PRIMITIVE_TEXTURE:
			BindTexture(bindable.ID);
			return;
		case JSH_GRAPHICS_PRIMITIVE_SAMPLER:
			BindSampler(bindable.ID);
			return;
		case JSH_GRAPHICS_PRIMITIVE_INVALID:
			return;
		default:
			jshLogE("Invalid bindable: %u", bindable.primitiveType);
			return;
		}

	}

	/////////////////////////BUFFER//////////////////////
	jsh::Buffer CreateBuffer(void* data, uint32 size, uint32 stride, JSH_USAGE usage, uint32 slot, JSH_BUFFER_TYPE bufferType, JSH_SHADER_TYPE constShaderType)
	{
		switch (jshGraphics::GetAPI()) {

		case JSH_GRAPHCS_API_DIRECTX11:
			return jshGraphics_dx11::CreateBuffer(data, size, stride, usage, slot, bufferType, constShaderType);

		case JSH_GRAPHCS_API_NULL:
		default:
			//TODO: throw exception
			jshDebug::ShowOkWindow(L"Undefined Graphics API", 3u);
			return INVALID_BUFFER;
		}
	}

	void BindVertexBuffer(jsh::Buffer buffer)
	{
		switch (jshGraphics::GetAPI())
		{

		case JSH_GRAPHCS_API_DIRECTX11:
			jshGraphics_dx11::BindVertexBuffer(buffer);
			return;
		case JSH_GRAPHCS_API_NULL:
		default:
			//TODO: throw exception
			jshDebug::ShowOkWindow(L"Undefined Graphics API", 3u);
			return;
		}
	}
	void BindIndexBuffer(jsh::Buffer buffer)
	{
		switch (jshGraphics::GetAPI())
		{
		case JSH_GRAPHCS_API_DIRECTX11:
			jshGraphics_dx11::BindIndexBuffer(buffer);
			return;
		case JSH_GRAPHCS_API_NULL:
		default:
			//TODO: throw exception
			jshDebug::ShowOkWindow(L"Undefined Graphics API", 3u);
			return;
		}
	}
	void BindConstantBuffer(jsh::Buffer buffer)
	{
		switch (jshGraphics::GetAPI())
		{

		case JSH_GRAPHCS_API_DIRECTX11:
			jshGraphics_dx11::BindConstantBuffer(buffer);
			return;
		case JSH_GRAPHCS_API_NULL:
		default:
			//TODO: throw exception
			jshDebug::ShowOkWindow(L"Undefined Graphics API", 3u);
			return;
		}
	}

	/////////////////////////INPUTLAYOUT//////////////////////
	jsh::InputLayout CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* descriptors, uint32 cant, jsh::VertexShader vs)
	{
		return jshGraphics_dx11::CreateInputLayout(descriptors, cant, vs);
	}
	void BindInputLayout(jsh::InputLayout inputLayout)
	{
		jshGraphics_dx11::BindInputLayout(inputLayout);
	}

	/////////////////////////SHADER//////////////////////
	jsh::VertexShader CreateVertexShader(const wchar* path)
	{
		return jshGraphics_dx11::CreateVertexShader(path);
	}
	jsh::VertexShader CreatePixelShader(const wchar* path)
	{
		return jshGraphics_dx11::CreatePixelShader(path);
	}
	void BindVertexShader(jsh::VertexShader vs)
	{
		jshGraphics_dx11::BindVertexShader(vs);
	}
	void BindPixelShader(jsh::PixelShader ps)
	{
		jshGraphics_dx11::BindPixelShader(ps);
	}

	/////////////////////////TEXTURE////////////////////////
	Texture CreateTexture(void* data, uint32 pitch, uint32 width, uint32 height, uint32 slot, JSH_FORMAT format, JSH_SHADER_TYPE shaderType, jsh::Sampler sampler)
	{
		return jshGraphics_dx11::CreateTexture(data, pitch, width, height, slot, format, shaderType, sampler);
	}
	void BindTexture(Texture texture)
	{
		return jshGraphics_dx11::BindTexture(texture);
	}

	/////////////////////////DEPTHSTENCILSTATE//////////////////////
	jsh::DepthStencilState CreateDepthStencilState(bool depth, bool stencil)
	{
		return jshGraphics_dx11::CreateDepthStencilState(depth, stencil);
	}
	void BindDepthStencilState(jsh::DepthStencilState state)
	{
		jshGraphics_dx11::BindDepthStencilState(state);
	}

	/////////////////////////SAMPLER////////////////////////
	jsh::Sampler CreateSampler(JSH_FILTER filter, JSH_TEXTURE_ADDRESS_MODE addressMode, uint32 slot, JSH_SHADER_TYPE shaderType)
	{
		return jshGraphics_dx11::CreateSampler(filter, addressMode, slot, shaderType);
	}
	void BindSampler(jsh::Sampler sampler)
	{
		jshGraphics_dx11::BindSampler(sampler);
	}

}

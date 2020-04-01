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
			BindVertexBuffer(bindable.ID, bindable.param0);
			return;
		case JSH_GRAPHICS_PRIMITIVE_INDEX_BUFFER:
			BindIndexBuffer(bindable.ID);
			return;
		case JSH_GRAPHICS_PRIMITIVE_CONSTANT_BUFFER:
			BindConstantBuffer(bindable.ID, bindable.param0, (JSH_SHADER_TYPE) bindable.param1);
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
		case JSH_GRAPHICS_PRIMITIVE_FRAME_BUFFER:
			BindFrameBuffer(bindable.ID);
			return;
		case JSH_GRAPHICS_PRIMITIVE_TEXTURE:
			BindTexture(bindable.ID, bindable.param0, (JSH_SHADER_TYPE) bindable.param1);
			return;
		case JSH_GRAPHICS_PRIMITIVE_INVALID:
			return;
		default:
			jshLogE("Invalid bindable: %u", bindable.primitiveType);
			return;
		}

	}

	/////////////////////////BUFFER//////////////////////
	jsh::Buffer CreateBuffer(void* data, uint32 size, uint32 stride, JSH_USAGE usage, JSH_BUFFER_TYPE bufferType)
	{
		switch (jshGraphics::GetAPI()) {

		case JSH_GRAPHCS_API_DIRECTX11:
			return jshGraphics_dx11::CreateBuffer(data, size, stride, usage, bufferType);

		case JSH_GRAPHCS_API_NULL:
		default:
			//TODO: throw exception
			jshDebug::ShowOkWindow(L"Undefined Graphics API", 3u);
			return INVALID_BUFFER;
		}
	}

	void BindVertexBuffer(jsh::Buffer buffer, uint32 slot)
	{
		switch (jshGraphics::GetAPI())
		{

		case JSH_GRAPHCS_API_DIRECTX11:
			jshGraphics_dx11::BindVertexBuffer(buffer, slot);
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
	void BindConstantBuffer(jsh::Buffer buffer, uint32 slot, JSH_SHADER_TYPE shaderType)
	{
		switch (jshGraphics::GetAPI())
		{

		case JSH_GRAPHCS_API_DIRECTX11:
			jshGraphics_dx11::BindConstantBuffer(buffer, slot, shaderType);
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
	Texture CreateTexture(void* data, uint32 pitch, uint32 width, uint32 height, JSH_FORMAT format)
	{
		return jshGraphics_dx11::CreateTexture(data, pitch, width, height, format);
	}
	void BindTexture(Texture texture, uint32 slot, JSH_SHADER_TYPE shaderType)
	{
		return jshGraphics_dx11::BindTexture(texture, slot, shaderType);
	}
	void SetSamplerState(jsh::Texture texture, JSH_FILTER filter, JSH_TEXTURE_ADDRESS_MODE addressMode)
	{
		jshGraphics_dx11::SetSamplerState(texture, filter, addressMode);
	}

	/////////////////////////FRAME BUFFER//////////////////////
	jsh::FrameBuffer CreateFrameBuffer(uint32 width, uint32 height)
	{
		return jshGraphics_dx11::CreateFrameBuffer(width, height);
	}
	void BindFrameBuffer(jsh::FrameBuffer fb)
	{
		jshGraphics_dx11::BindFrameBuffer(fb);
	}
	void SetDepthState(bool enable, jsh::FrameBuffer fb)
	{
		jshGraphics_dx11::SetDepthState(enable, fb);
	}
	void SetStencilState(bool enable, jsh::FrameBuffer fb)
	{
		jshGraphics_dx11::SetStencilState(enable, fb);
	}

}

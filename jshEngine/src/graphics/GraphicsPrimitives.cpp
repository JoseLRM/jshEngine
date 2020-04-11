#include "GraphicsPrimitives.h"

#include "..//Debug.h"

#include "Graphics.h"
#include "DirectX/GraphicsAPI_dx11.h"

using namespace jsh;

namespace jshGraphics {

	RenderTargetView g_RenderTargetView[1];

	void Bind(const Bindable& bindable, CommandList cmd)
	{
		switch (bindable.type)
		{
		case JSH_PRIMITIVE_VERTEX_BUFFER:
			jshGraphics::BindVertexBuffer(*(Buffer*)bindable.primitive.get(), bindable.param0, cmd);
			break;
		case JSH_PRIMITIVE_INDEX_BUFFER:
			jshGraphics::BindIndexBuffer(*(Buffer*)bindable.primitive.get(), cmd);
			break;
		case JSH_PRIMITIVE_CONSTANT_BUFFER:
			jshGraphics::BindConstantBuffer(*(Buffer*)bindable.primitive.get(), bindable.param0, (JSH_SHADER_TYPE)bindable.param1, cmd);
			break;
		case JSH_PRIMITIVE_VERTEX_SHADER:
			jshGraphics::BindVertexShader(*(VertexShader*)bindable.primitive.get(), cmd);
			break;
		case JSH_PRIMITIVE_PIXEL_SHADER:
			jshGraphics::BindPixelShader(*(PixelShader*)bindable.primitive.get(), cmd);
			break;
		case JSH_PRIMITIVE_GEOMETRY_SHADER:
			//jshGraphics::BindVertexShader(*(VertexShader*)bindable.primitive.get(), cmd);
			//TODO: all types of shaders
			break;
		case JSH_PRIMITIVE_INPUT_LAYOUT:
			jshGraphics::BindInputLayout(*(InputLayout*)bindable.primitive.get(), cmd);
			break;
		case JSH_PRIMITIVE_TEXTURE:
			jshGraphics::BindTexture(*(Texture*)bindable.primitive.get(), bindable.param0, (JSH_SHADER_TYPE)bindable.param1, cmd);
			break;
		case JSH_PRIMITIVE_SAMPLER_STATE:
			jshGraphics::BindSamplerState(*(SamplerState*)bindable.primitive.get(), bindable.param0, (JSH_SHADER_TYPE)bindable.param1, cmd);
			break;
		case JSH_PRIMITIVE_BLEND_STATE:
			jshGraphics::BindBlendState(*(BlendState*)bindable.primitive.get(), cmd);
			break;
		case JSH_PRIMITIVE_DEPTHSTENCIL_STATE:
			jshGraphics::BindDepthStencilState(*(DepthStencilState*)bindable.primitive.get(), cmd);
			break;
		case JSH_PRIMITIVE_RASTERIZER_STATE:
			jshGraphics::BindRasterizerState(*(RasterizerState*)bindable.primitive.get(), cmd);
			break;
		case JSH_PRIMITIVE_RENDER_TARGET_VIEW:
			jshGraphics::BindRenderTargetView(*(RenderTargetView*)bindable.primitive.get(), cmd);
			break;
		}
	}

	/////////////////////////BUFFER//////////////////////
	void CreateBuffer(const JSH_BUFFER_DESC* desc, JSH_SUBRESOURCE_DATA* sdata, jsh::Buffer* buffer)
	{
		switch (jshGraphics::GetAPI()) {

		case JSH_GRAPHCS_API_DIRECTX11:
			jshGraphics_dx11::CreateBuffer(desc, sdata, buffer);
			return;
		case JSH_GRAPHCS_API_NULL:
		default:
			throw jshGfxNullAPIException;
		}
	}

	void BindVertexBuffer(const jsh::Buffer& buffer, uint32 slot, CommandList cmd)
	{
		switch (jshGraphics::GetAPI())
		{

		case JSH_GRAPHCS_API_DIRECTX11:
			jshGraphics_dx11::BindVertexBuffer(buffer, slot, cmd);
			return;
		case JSH_GRAPHCS_API_NULL:
		default:
			throw jshGfxNullAPIException;
			return;
		}
	}
	void BindIndexBuffer(const jsh::Buffer& buffer, CommandList cmd)
	{
		switch (jshGraphics::GetAPI())
		{
		case JSH_GRAPHCS_API_DIRECTX11:
			jshGraphics_dx11::BindIndexBuffer(buffer, cmd);
			return;
		case JSH_GRAPHCS_API_NULL:
		default:
			throw jshGfxNullAPIException;
			return;
		}
	}
	void BindConstantBuffer(const jsh::Buffer& buffer, uint32 slot, JSH_SHADER_TYPE shaderType, CommandList cmd)
	{
		switch (jshGraphics::GetAPI())
		{
		case JSH_GRAPHCS_API_DIRECTX11:
			jshGraphics_dx11::BindConstantBuffer(buffer, slot, shaderType, cmd);
			return;
		case JSH_GRAPHCS_API_NULL:
		default:
			throw jshGfxNullAPIException;
			return;
		}
	}

	/////////////////////////INPUTLAYOUT//////////////////////
	void CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* descriptors, uint32 cant, jsh::VertexShader& vs, jsh::InputLayout* il)
	{
		jshGraphics_dx11::CreateInputLayout(descriptors, cant, vs, il);
	}
	void BindInputLayout(const jsh::InputLayout& inputLayout, CommandList cmd)
	{
		jshGraphics_dx11::BindInputLayout(inputLayout, cmd);
	}

	/////////////////////////SHADER//////////////////////
	void CreateVertexShader(const wchar* path, jsh::VertexShader* vs)
	{
		jshGraphics_dx11::CreateVertexShader(path, vs);
	}
	void CreatePixelShader(const wchar* path, jsh::PixelShader* ps)
	{
		jshGraphics_dx11::CreatePixelShader(path, ps);
	}
	void BindVertexShader(const jsh::VertexShader& vs, CommandList cmd)
	{
		jshGraphics_dx11::BindVertexShader(vs, cmd);
	}
	void BindPixelShader(const jsh::PixelShader& ps, CommandList cmd)
	{
		jshGraphics_dx11::BindPixelShader(ps, cmd);
	}

	/////////////////////////TEXTURE////////////////////////
	void CreateTexture(const JSH_TEXTURE2D_DESC* desc, JSH_SUBRESOURCE_DATA* sdata, jsh::Texture* tex)
	{
		jshGraphics_dx11::CreateTexture(desc, sdata, tex);
	}
	void BindTexture(const Texture& texture, uint32 slot, JSH_SHADER_TYPE shaderType, CommandList cmd)
	{
		jshGraphics_dx11::BindTexture(texture, slot, shaderType, cmd);
	}

	/////////////////////////VIEWPORT////////////////////////
	void CreateViewport(float x, float y, float width, float height, jsh::Viewport* vp)
	{
		jshGraphics_dx11::CreateViewport(x, y, width, height, vp);
	}
	void BindViewport(const Viewport& viewport, uint32 slot, CommandList cmd)
	{
		jshGraphics_dx11::BindViewport(viewport, slot, cmd);
	}

	/////////////////////////SAMPLER STATE//////////////////////
	void CreateSamplerState(const JSH_SAMPLER_DESC* desc, jsh::SamplerState* ss)
	{
		jshGraphics_dx11::CreateSamplerState(desc, ss);
	}
	void BindSamplerState(const jsh::SamplerState& ss, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd)
	{
		jshGraphics_dx11::BindSamplerState(ss, slot, shaderType, cmd);
	}
	/////////////////////////BLEND STATE//////////////////////
	void CreateBlendState(const JSH_BLEND_DESC* desc, jsh::BlendState* bs)
	{
		return jshGraphics_dx11::CreateBlendState(desc, bs);
	}
	void BindBlendState(const BlendState& bs, CommandList cmd)
	{
		jshGraphics_dx11::BindBlendState(bs, cmd);
	}

	/////////////////////////DEPTHSTENCIL STATE//////////////////////
	void CreateDepthStencilState(const JSH_DEPTH_STENCIL_DESC* desc, jsh::DepthStencilState* dss)
	{
		return jshGraphics_dx11::CreateDepthStencilState(desc, dss);
	}
	void BindDepthStencilState(const DepthStencilState& dsState, CommandList cmd)
	{
		jshGraphics_dx11::BindDepthStencilState(dsState, cmd);
	}
	void ClearDepthStencilView(const jsh::Texture& tex, jsh::CommandList cmd)
	{
		jshGraphics_dx11::ClearDepthStencilView(tex, cmd);
	}

	/////////////////////////RASTERIZER STATE//////////////////////
	void CreateRasterizerState(const JSH_RASTERIZER_DESC* desc, jsh::RasterizerState* rs)
	{
		return jshGraphics_dx11::CreateRasterizerState(desc, rs);
	}
	void BindRasterizerState(const RasterizerState& rasterizerState, CommandList cmd)
	{
		jshGraphics_dx11::BindRasterizerState(rasterizerState, cmd);
	}

	/////////////////////////RENDER TARGET VIEW////////////////////////
	void CreateRenderTargetView(const JSH_RENDER_TARGET_VIEW_DESC* desc, const JSH_TEXTURE2D_DESC* texDesc, jsh::RenderTargetView* rtv)
	{
		return jshGraphics_dx11::CreateRenderTargetView(desc, texDesc, rtv);
	}
	void CreateRenderTargetViewFromBackBuffer(jsh::RenderTargetView* rtv)
	{
		return jshGraphics_dx11::CreateRenderTargetViewFromBackBuffer(rtv);
	}
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, jsh::CommandList cmd)
	{
		jshGraphics_dx11::BindRenderTargetView(rtv, cmd);
		g_RenderTargetView[0] = rtv;
	}
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, const jsh::Texture& tex, jsh::CommandList cmd)
	{
		jshGraphics_dx11::BindRenderTargetView(rtv, tex, cmd);
		g_RenderTargetView[0] = rtv;
	}
	void ClearRenderTargetView(const jsh::RenderTargetView& rtv, jsh::CommandList cmd)
	{
		jshGraphics_dx11::ClearRenderTargetView(rtv, cmd);
	}
	RenderTargetView& GetRenderTargetView()
	{
		return g_RenderTargetView[0];
	}
}

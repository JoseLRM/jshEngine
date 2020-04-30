#include "GraphicsPrimitives.h"

#include "Debug.h"

#include "Graphics.h"
#include "GraphicsAPI_dx11.h"
#include "EventSystem.h"

using namespace jsh;

namespace jshGraphics {

	CommandList BeginCommandList()
	{
		return jshGraphics_dx11::BeginCommandList();
	}
	/////////////////////////DRAW CALLS//////////////////////
	void DrawIndexed(uint32 indicesCount, CommandList cmd)
	{
		jshGraphics_dx11::DrawIndexed(indicesCount, cmd);
	}
	void Draw(uint32 vertexCount, jsh::CommandList cmd)
	{
		jshGraphics_dx11::Draw(vertexCount, cmd);
	}
	void DrawInstanced(uint32 vertexPerInstance, uint32 instances, uint32 startVertex, uint32 startInstance, jsh::CommandList cmd)
	{
		jshGraphics_dx11::DrawInstanced(vertexPerInstance, instances, startVertex, startInstance, cmd);
	}

	/////////////////////////TOPOLOGY//////////////////////
	void SetTopology(JSH_TOPOLOGY topology, jsh::CommandList cmd) {
		jshGraphics_dx11::SetTopology(topology, cmd);
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
	void UpdateBuffer(jsh::Buffer& res, void* data, uint32 size, jsh::CommandList cmd)
	{
		jshGraphics_dx11::UpdateBuffer(res, data, size, cmd);
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
	void CreateTextureRes(const JSH_TEXTURE2D_DESC* desc, JSH_SUBRESOURCE_DATA* sdata, jsh::TextureRes* tex)
	{
		jshGraphics_dx11::CreateTextureRes(desc, sdata, tex);
	}
	void BindTexture(const TextureRes& texture, uint32 slot, JSH_SHADER_TYPE shaderType, CommandList cmd)
	{
		jshGraphics_dx11::BindTexture(texture, slot, shaderType, cmd);
	}
	void BindTexture(const jsh::RenderTargetView& rtv, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd)
	{
		jshGraphics_dx11::BindTexture(rtv, slot, shaderType, cmd);
	}
	void UnbindTexture(uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd)
	{
		jshGraphics_dx11::UnbindTexture(slot, shaderType, cmd);
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
	void BindDepthStencilState(const DepthStencilState& dsState, uint32 stencilRef, CommandList cmd)
	{
		jshGraphics_dx11::BindDepthStencilState(dsState, stencilRef, cmd);
	}
	void ClearDepthStencilView(const jsh::TextureRes& tex, jsh::CommandList cmd)
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
	jsh::RenderTargetView& GetRenderTargetViewFromBackBuffer()
	{
		return jshGraphics_dx11::GetRenderTargetViewFromBackBuffer();
	}
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, jsh::CommandList cmd)
	{
		jshGraphics_dx11::BindRenderTargetView(rtv, cmd);
	}
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, const jsh::TextureRes& tex, jsh::CommandList cmd)
	{
		jshGraphics_dx11::BindRenderTargetView(rtv, tex, cmd);
	}
	void ClearRenderTargetView(const jsh::RenderTargetView& rtv, float r, float g, float b, float a, jsh::CommandList cmd)
	{
		jshGraphics_dx11::ClearRenderTargetView(rtv, r, g, b, a, cmd);
	}

	//////////////////////////DEFAULT PRIMITIVES///////////////////////
	void primitives::Initialize()
	{
		// QUAD 2D BUFFER
		{
			float data[] = {
				-0.5f,  0.5f,
				 0.5f,  0.5f,
				-0.5f, -0.5f,
				 0.5f, -0.5f
			};

			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_VERTEX_BUFFER;
			desc.ByteWidth = 8 * sizeof(float);
			desc.CPUAccessFlags = 0u;
			desc.MiscFlags = 0u;
			desc.StructureByteStride = 2 * sizeof(float);
			desc.Usage = JSH_USAGE_IMMUTABLE;
			JSH_SUBRESOURCE_DATA sdata;
			sdata.pSysMem = data;
			jshGraphics::CreateBuffer(&desc, &sdata, &s_QuadBuffer);
		}
		// MAIN FRAME BUFFER
		{
			JSH_RENDER_TARGET_VIEW_DESC rtvDesc;
			jshZeroMemory(&rtvDesc, sizeof(JSH_RENDER_TARGET_VIEW_DESC));
			rtvDesc.Format = JSH_FORMAT_B8G8R8A8_UNORM;
			rtvDesc.ViewDimension = JSH_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0u;

			JSH_DEPTH_STENCIL_DESC dsDesc;
			jshZeroMemory(&dsDesc, sizeof(JSH_DEPTH_STENCIL_DESC));
			dsDesc.DepthEnable = true;
			dsDesc.DepthFunc = JSH_COMPARISON_LESS;
			dsDesc.DepthWriteMask = JSH_DEPTH_WRITE_MASK_ALL;
			dsDesc.StencilEnable = false;

			JSH_TEXTURE2D_DESC dsResDesc;
			jshZeroMemory(&dsResDesc, sizeof(JSH_TEXTURE2D_DESC));
			dsResDesc.ArraySize = 1u;
			dsResDesc.BindFlags = JSH_BIND_DEPTH_STENCIL;
			dsResDesc.CPUAccessFlags = 0u;
			dsResDesc.Format = JSH_FORMAT_D24_UNORM_S8_UINT;
			dsResDesc.Width = 1080;
			dsResDesc.Height = 720;
			dsResDesc.MipLevels = 1u;
			dsResDesc.MiscFlags = 0u;
			dsResDesc.SampleDesc.Count = 1u;
			dsResDesc.SampleDesc.Quality = 0u;
			dsResDesc.Usage = JSH_USAGE_DEFAULT;

			jshGraphics::CreateDepthStencilState(&dsDesc, &s_DefaultDepthStencilState);
			jshGraphics::CreateTextureRes(&dsResDesc, nullptr, &s_DefaultDepthStencilView);
		}

		// DISABLED DSS
		{
			JSH_DEPTH_STENCIL_DESC desc;
			jshZeroMemory(&desc, sizeof(JSH_DEPTH_STENCIL_DESC));
			desc.DepthEnable = false;
			desc.StencilEnable = false;
			desc.DepthWriteMask = JSH_DEPTH_WRITE_MASK_ZERO;
			jshGraphics::CreateDepthStencilState(&desc, &s_DisabledDepthStencilState);
		}

		// OFFSREEN RTV
		{
			JSH_RENDER_TARGET_VIEW_DESC rtvDesc;
			rtvDesc.Format = JSH_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.Texture2D.MipSlice = 0u;
			rtvDesc.ViewDimension = JSH_RTV_DIMENSION_TEXTURE2D;

			JSH_TEXTURE2D_DESC res;
			res.ArraySize = 1u;
			res.BindFlags = JSH_BIND_RENDER_TARGET | JSH_BIND_SHADER_RESOURCE;
			res.CPUAccessFlags = 0u;
			res.Format = JSH_FORMAT_R8G8B8A8_UNORM;
			res.Width = 1080;
			res.Height = 720;
			res.MipLevels = 1u;
			res.MiscFlags = 0u;
			res.SampleDesc.Count = 1u;
			res.SampleDesc.Quality = 0u;
			res.Usage = JSH_USAGE_DEFAULT;

			jshGraphics::CreateRenderTargetView(&rtvDesc, &res, &s_OffscreenRenderTargetView);
		}

		// DEFAULT SAMPLER & VIEWPORT
		{
			JSH_SAMPLER_DESC samplerDesc;
			jshZeroMemory(&samplerDesc, sizeof(JSH_SAMPLER_DESC));
			samplerDesc.AddressU = JSH_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = JSH_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = JSH_TEXTURE_ADDRESS_WRAP;
			samplerDesc.Filter = JSH_FILTER_MIN_MAG_MIP_LINEAR;
			jshGraphics::CreateSamplerState(&samplerDesc, &s_DefaultSamplerState);
			jshGraphics::CreateViewport(0, 0, 1080, 720, &s_DefaultViewport);
		}

		// DEFAULT BLEND STATES
		{
			JSH_BLEND_DESC desc;
			jshZeroMemory(&desc, sizeof(JSH_BLEND_DESC));
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = JSH_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = JSH_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = JSH_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = JSH_COLOR_WRITE_ENABLE_ALL;

			desc.RenderTarget[0].SrcBlendAlpha = JSH_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = JSH_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = JSH_BLEND_OP_ADD;

			jshGraphics::CreateBlendState(&desc, &s_TransparentBlendState);
		}
		{
			JSH_BLEND_DESC desc;
			jshZeroMemory(&desc, sizeof(JSH_BLEND_DESC));
			desc.RenderTarget[0].BlendEnable = false;
			desc.RenderTarget[0].RenderTargetWriteMask = JSH_COLOR_WRITE_ENABLE_ALL;

			jshGraphics::CreateBlendState(&desc, &s_DefaultBlendState);
		}

		// CHANGE RESOLUTION
		jshEvent::Register<ResolutionEvent>(JSH_EVENT_LAYER_SYSTEM, [](ResolutionEvent& e) {
			
			jshGraphics::CreateViewport(0.f, 0.f, e.resolution.x, e.resolution.y, &s_DefaultViewport);

			s_OffscreenRenderTargetView.resDesc.Width = e.resolution.x;
			s_OffscreenRenderTargetView.resDesc.Height = e.resolution.y;
			jshGraphics::CreateRenderTargetView(&s_OffscreenRenderTargetView.desc, &s_OffscreenRenderTargetView.resDesc, &s_OffscreenRenderTargetView);
			
			return true;
		});
	}

	jsh::Buffer primitives::s_QuadBuffer;

	jsh::DepthStencilState primitives::s_DefaultDepthStencilState;
	jsh::DepthStencilState primitives::s_DisabledDepthStencilState;

	jsh::TextureRes primitives::s_DefaultDepthStencilView;
	jsh::RenderTargetView primitives::s_OffscreenRenderTargetView;

	jsh::SamplerState primitives::s_DefaultSamplerState;
	jsh::Viewport primitives::s_DefaultViewport;

	jsh::BlendState primitives::s_TransparentBlendState;
	jsh::BlendState primitives::s_DefaultBlendState;


}

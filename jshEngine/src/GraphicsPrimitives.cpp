#include "common.h"

#include "GraphicsPrimitives.h"

#include "Graphics.h"
#include "GraphicsAPI_dx11.h"
#include "EventSystem.h"

#ifdef JSH_IMGUI
#include "Window.h"
#endif 


using namespace jsh;
using namespace jshGraphics::_internal;

namespace jshGraphics {

	std::vector<GraphicsPrimitive_internal*> g_Primitives;
	std::mutex g_PrimitivesMutex;
	uint32 g_PrimitivesReleasedCount = 0u;

	namespace _internal {
		void AllocatePrimitive(GraphicsPrimitive_internal* primitive)
		{
			std::lock_guard<std::mutex> lock(g_PrimitivesMutex);
			primitive->ID = g_Primitives.size() + 1u;
			g_Primitives.push_back(primitive);
		}
		void ReleasePrimitive(GraphicsPrimitive_internal* primitive)
		{
			JSH_ASSERT(primitive->ID != 0u);
			primitive->Release();

			std::lock_guard<std::mutex> lock(g_PrimitivesMutex);
			g_Primitives[primitive->ID - 1] = nullptr;
			delete primitive;
			g_PrimitivesReleasedCount++;

			if (g_PrimitivesReleasedCount > 10) {
				for (int32 i = g_Primitives.size() - 1; i >= 0; --i) {
					if (g_Primitives[i] == nullptr) g_Primitives.erase(g_Primitives.begin() + i);
				}
				for (uint32 i = 0; i < g_Primitives.size(); ++i) {
					g_Primitives[i]->ID = i + 1u;
				}
				g_PrimitivesReleasedCount = 0u;
			}
		}
		void ReleasePrimitives()
		{
			std::lock_guard<std::mutex> lock(g_PrimitivesMutex);
			for (auto it = g_Primitives.begin(); it != g_Primitives.end(); ++it) {
				if ((*it) != nullptr) {
					(*it)->Release();
					delete (*it);
				}
			}
			g_Primitives.clear();
		}
	}

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
	void DrawIndexedInstanced(uint32 indexPerInstance, uint32 instances, uint32 startIndex, uint32 startVertex, uint32 startInstance, jsh::CommandList cmd)
	{
		jshGraphics_dx11::DrawIndexedInstanced(indexPerInstance, instances, startIndex, startVertex, startInstance, cmd);
	}

	/////////////////////////TOPOLOGY//////////////////////
	void SetTopology(JSH_TOPOLOGY topology, jsh::CommandList cmd) {
		jshGraphics_dx11::SetTopology(topology, cmd);
	}
	/////////////////////////BUFFER//////////////////////
	void CreateBuffer(const JSH_BUFFER_DESC* desc, JSH_SUBRESOURCE_DATA* sdata, jsh::Buffer* buffer)
	{
		if (buffer->internalAllocation) {
			ReleasePrimitive(reinterpret_cast<Buffer_Internal*>(buffer->internalAllocation));
		}
		jshGraphics_dx11::CreateBuffer(desc, sdata, buffer);
		Buffer_Internal* b = reinterpret_cast<Buffer_Internal*>(buffer->internalAllocation);
		b->desc = *desc;
		b->type = JSH_RESOURCE_TYPE_BUFFER;
		_internal::AllocatePrimitive(b);
	}

	void BindVertexBuffers(const jsh::Buffer** buffers, uint32 slot, uint32 count, const uint32* strides, const uint32* offsets, jsh::CommandList cmd)
	{
		jshGraphics_dx11::BindVertexBuffers(buffers, slot, count, strides, offsets, cmd);
	}
	void BindIndexBuffer(const jsh::Buffer& buffer, JSH_FORMAT format, uint32 offset, jsh::CommandList cmd)
	{
		jshGraphics_dx11::BindIndexBuffer(buffer, format, offset, cmd);
	}
	void BindConstantBuffers(const jsh::Buffer** buffers, uint32 slot, uint32 count, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd)
	{
		jshGraphics_dx11::BindConstantBuffers(buffers, slot, count, shaderType, cmd);
	}
	const JSH_BUFFER_DESC& GetBufferDesc(const jsh::Buffer& buffer)
	{
		Buffer_Internal* b = reinterpret_cast<Buffer_Internal*>(buffer.internalAllocation);
		return b->desc;
	}
	void UpdateBuffer(jsh::Buffer& res, void* data, uint32 size, jsh::CommandList cmd)
	{
		jshGraphics_dx11::UpdateBuffer(res, data, size, cmd);
	}

	/////////////////////////INPUTLAYOUT//////////////////////
	void CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* descriptors, uint32 cant, jsh::VertexShader& vs, jsh::InputLayout* il)
	{
		if (il->internalAllocation) {
			ReleasePrimitive(reinterpret_cast<InputLayout_Internal*>(il->internalAllocation));
		}
		jshGraphics_dx11::CreateInputLayout(descriptors, cant, vs, il);
		_internal::AllocatePrimitive(reinterpret_cast<InputLayout_Internal*>(il->internalAllocation));
	}
	void BindInputLayout(const jsh::InputLayout& inputLayout, CommandList cmd)
	{
		jshGraphics_dx11::BindInputLayout(inputLayout, cmd);
	}

	/////////////////////////SHADER//////////////////////
	void CreateVertexShader(const wchar* path, jsh::VertexShader* vs)
	{
		if (vs->internalAllocation) {
			ReleasePrimitive(reinterpret_cast<VertexShader_Internal*>(vs->internalAllocation));
		}
		jshGraphics_dx11::CreateVertexShader(path, vs);
		_internal::AllocatePrimitive(reinterpret_cast<VertexShader_Internal*>(vs->internalAllocation));
	}
	void CreatePixelShader(const wchar* path, jsh::PixelShader* ps)
	{
		if (ps->internalAllocation) {
			ReleasePrimitive(reinterpret_cast<PixelShader_Internal*>(ps->internalAllocation));
		}
		jshGraphics_dx11::CreatePixelShader(path, ps);
		_internal::AllocatePrimitive(reinterpret_cast<PixelShader_Internal*>(ps->internalAllocation));
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
		if (tex->internalAllocation) {
			ReleasePrimitive(reinterpret_cast<TextureRes_Internal*>(tex->internalAllocation));
		}
		jshGraphics_dx11::CreateTextureRes(desc, sdata, tex);
		TextureRes_Internal* t = reinterpret_cast<TextureRes_Internal*>(tex->internalAllocation);
		t->desc = *desc;
		t->type = JSH_RESOURCE_TYPE_TEXTURE2D;
		_internal::AllocatePrimitive(t);
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

	void UpdateTexture(jsh::TextureRes& res, void* data, uint32 size, jsh::CommandList cmd)
	{
		jshGraphics_dx11::UpdateTexture(res, data, size, cmd);
	}

	const JSH_TEXTURE2D_DESC& GetTextureDesc(const jsh::TextureRes& res)
	{
		TextureRes_Internal* t = reinterpret_cast<TextureRes_Internal*>(res.internalAllocation);
		return t->desc;
	}

	/////////////////////////VIEWPORT////////////////////////
	void CreateViewport(float x, float y, float width, float height, jsh::Viewport* vp)
	{
		if (vp->internalAllocation) {
			ReleasePrimitive(reinterpret_cast<Viewport_Internal*>(vp->internalAllocation));
		}
		jshGraphics_dx11::CreateViewport(x, y, width, height, vp);
		_internal::AllocatePrimitive(reinterpret_cast<Viewport_Internal*>(vp->internalAllocation));
	}
	void BindViewport(const Viewport& viewport, uint32 slot, CommandList cmd)
	{
		jshGraphics_dx11::BindViewport(viewport, slot, cmd);
	}

	/////////////////////////SAMPLER STATE//////////////////////
	void CreateSamplerState(const JSH_SAMPLER_DESC* desc, jsh::SamplerState* ss)
	{
		if (ss->internalAllocation) {
			ReleasePrimitive(reinterpret_cast<SamplerState_Internal*>(ss->internalAllocation));
		}
		jshGraphics_dx11::CreateSamplerState(desc, ss);
		_internal::AllocatePrimitive(reinterpret_cast<SamplerState_Internal*>(ss->internalAllocation));
	}
	void BindSamplerState(const jsh::SamplerState& ss, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd)
	{
		jshGraphics_dx11::BindSamplerState(ss, slot, shaderType, cmd);
	}
	/////////////////////////BLEND STATE//////////////////////
	void CreateBlendState(const JSH_BLEND_DESC* desc, jsh::BlendState* bs)
	{
		if (bs->internalAllocation) {
			ReleasePrimitive(reinterpret_cast<BlendState_Internal*>(bs->internalAllocation));
		}
		jshGraphics_dx11::CreateBlendState(desc, bs);
		_internal::AllocatePrimitive(reinterpret_cast<BlendState_Internal*>(bs->internalAllocation));
	}
	void BindBlendState(const BlendState& bs, CommandList cmd)
	{
		jshGraphics_dx11::BindBlendState(bs, cmd);
	}

	/////////////////////////DEPTHSTENCIL STATE//////////////////////
	void CreateDepthStencilState(const JSH_DEPTH_STENCIL_DESC* desc, jsh::DepthStencilState* dss)
	{
		if (dss->internalAllocation) {
			ReleasePrimitive(reinterpret_cast<DepthStencilState_Internal*>(dss->internalAllocation));
		}
		jshGraphics_dx11::CreateDepthStencilState(desc, dss);
		_internal::AllocatePrimitive(reinterpret_cast<DepthStencilState_Internal*>(dss->internalAllocation));
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
		if (rs->internalAllocation) {
			ReleasePrimitive(reinterpret_cast<RasterizerState_Internal*>(rs->internalAllocation));
		}
		jshGraphics_dx11::CreateRasterizerState(desc, rs);
		_internal::AllocatePrimitive(reinterpret_cast<RasterizerState_Internal*>(rs->internalAllocation));
	}
	void BindRasterizerState(const RasterizerState& rasterizerState, CommandList cmd)
	{
		jshGraphics_dx11::BindRasterizerState(rasterizerState, cmd);
	}

	/////////////////////////RENDER TARGET VIEW////////////////////////
	void CreateRenderTargetView(const JSH_RENDER_TARGET_VIEW_DESC* desc, const JSH_TEXTURE2D_DESC* texDesc, jsh::RenderTargetView* rtv)
	{
		if (rtv->internalAllocation) {
			ReleasePrimitive(reinterpret_cast<RenderTargetView_Internal*>(rtv->internalAllocation));
		}
		jshGraphics_dx11::CreateRenderTargetView(desc, texDesc, rtv);
		RenderTargetView_Internal* RTV = reinterpret_cast<RenderTargetView_Internal*>(rtv->internalAllocation);
		RTV->desc = *desc;
		RTV->resDesc = *texDesc;
		_internal::AllocatePrimitive(RTV);
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

	void ResizeRenderTargetView(RenderTargetView& rtv, uint32 width, uint32 height)
	{
		JSH_TEXTURE2D_DESC resdesc = GetRenderTargetTextureDesc(rtv);
		resdesc.Width = width;
		resdesc.Height = height;
		JSH_RENDER_TARGET_VIEW_DESC desc = GetRenderTargetViewDesc(rtv);
		jshGraphics::CreateRenderTargetView(&desc, &resdesc, &rtv);
	}

	const JSH_RENDER_TARGET_VIEW_DESC& GetRenderTargetViewDesc(const jsh::RenderTargetView& rtv)
	{
		RenderTargetView_Internal* RTV = reinterpret_cast<RenderTargetView_Internal*>(rtv.internalAllocation);
		return RTV->desc;
	}
	const JSH_TEXTURE2D_DESC& GetRenderTargetTextureDesc(const jsh::RenderTargetView& rtv)
	{
		RenderTargetView_Internal* RTV = reinterpret_cast<RenderTargetView_Internal*>(rtv.internalAllocation);
		return RTV->resDesc;
	}

	//////////////////////////DEFAULT PRIMITIVES///////////////////////
	void primitives::Initialize()
	{
		// MAIN FRAME BUFFER
		{
			JSH_DEPTH_STENCIL_DESC dsDesc;
			jshZeroMemory(&dsDesc, sizeof(JSH_DEPTH_STENCIL_DESC));
			dsDesc.DepthEnable = true;
			dsDesc.DepthFunc = JSH_COMPARISON_LESS_EQUAL;
			dsDesc.DepthWriteMask = JSH_DEPTH_WRITE_MASK_ALL;
			dsDesc.StencilEnable = false;

			jshGraphics::CreateDepthStencilState(&dsDesc, &s_DefaultDepthStencilState);
		}

		// DISABLED DSS
		{
			JSH_DEPTH_STENCIL_DESC desc;
			jshZeroMemory(&desc, sizeof(JSH_DEPTH_STENCIL_DESC));
			desc.DepthEnable = false;
			desc.StencilEnable = false;
			desc.DepthFunc = JSH_COMPARISON_ALWAYS;
			desc.DepthWriteMask = JSH_DEPTH_WRITE_MASK_ZERO;
			jshGraphics::CreateDepthStencilState(&desc, &s_DisabledDepthStencilState);
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
			jshGraphics::CreateViewport(0, 0, float(jshGraphics::GetResolutionWidth()), float(jshGraphics::GetResolutionHeight()), &s_DefaultViewport);
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

		static jshEvent::Listener<OutputModeEvent> listener;
		// CHANGE RESOLUTION
		listener.Register(JSH_EVENT_LAYER_SYSTEM, [](OutputModeEvent& e) {
			
			jshGraphics::CreateViewport(0.f, 0.f, float(e.outputMode.resolution.x), float(e.outputMode.resolution.y), &s_DefaultViewport);
			return true;
		});
	}

	jsh::DepthStencilState primitives::s_DefaultDepthStencilState;
	jsh::DepthStencilState primitives::s_DisabledDepthStencilState;

	jsh::SamplerState primitives::s_DefaultSamplerState;
	jsh::Viewport primitives::s_DefaultViewport;

	jsh::BlendState primitives::s_TransparentBlendState;
	jsh::BlendState primitives::s_DefaultBlendState;


}


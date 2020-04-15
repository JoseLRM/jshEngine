#include "GraphicsAPI_dx11.h"

#include "..//WinLib.h"
#include "..//Window.h"
#include "DirectX11Lib.h"

#include "..//..//utils/dataStructures/safe_queue.h"
#include "..//..//utils/dataStructures/vector.h"
#include "..//..//Debug.h"

#include "..//..//ImGui/imgui.h"
#include "..//..//ImGui/imgui_impl_win32.h"
#include "..//..//ImGui/imgui_impl_dx11.h"

namespace jshGraphics_dx11 {

	//////////////////GRAPHICS PRIMITIVES////////////////////

	struct Buffer_dx11 {
		ComPtr<ID3D11Buffer> ptr;
		uint32 structureByteStride;
	};

	struct InputLayout_dx11 {
		ComPtr<ID3D11InputLayout> ptr;
	};

	struct VertexShader_dx11 {
		ComPtr<ID3D11VertexShader> ptr;
		ComPtr<ID3DBlob> blob;
	};

	struct PixelShader_dx11 {
		ComPtr<ID3D11PixelShader> ptr;
	};

	struct Texture_dx11 {
		ComPtr<ID3D11Texture2D> texturePtr;
		ComPtr<ID3D11ShaderResourceView> shaderSrcView;
		ComPtr<ID3D11DepthStencilView> depthStencilView;
	};

	typedef D3D11_VIEWPORT Viewport_dx11;

	struct SamplerState_dx11 {
		ComPtr<ID3D11SamplerState> ptr;
	};

	struct BlendState_dx11 {
		ComPtr<ID3D11BlendState> ptr;
	};

	struct DepthStencilState_dx11 {
		ComPtr<ID3D11DepthStencilState> statePtr;
	};

	struct RasterizerState_dx11 {
		ComPtr<ID3D11RasterizerState> ptr;
	};

	struct RenderTargetView_dx11 {
		ComPtr<ID3D11RenderTargetView> ptr;
		ComPtr<ID3D11Texture2D> resourcePtr;
		ComPtr<ID3D11ShaderResourceView> shaderResView;
	};

#define jshDefineToInternal(jshFormat, dx11Format) inline dx11Format* ToInternal(const jshFormat& primitive) \
	{ return (dx11Format*) primitive.internalAllocation.get(); }

	jshDefineToInternal(jsh::Buffer, Buffer_dx11)
	jshDefineToInternal(jsh::InputLayout, InputLayout_dx11)
	jshDefineToInternal(jsh::Texture, Texture_dx11)
	jshDefineToInternal(jsh::VertexShader, VertexShader_dx11)
	jshDefineToInternal(jsh::PixelShader, PixelShader_dx11)
	jshDefineToInternal(jsh::Viewport, Viewport_dx11)
	jshDefineToInternal(jsh::SamplerState, SamplerState_dx11)
	jshDefineToInternal(jsh::BlendState, BlendState_dx11)
	jshDefineToInternal(jsh::DepthStencilState, DepthStencilState_dx11)
	jshDefineToInternal(jsh::RasterizerState, RasterizerState_dx11)
	jshDefineToInternal(jsh::RenderTargetView, RenderTargetView_dx11)

	////////////////////ALLOCATION//////////////////////////

	ComPtr<ID3D11Device> g_Device = nullptr;
	ComPtr<ID3D11DeviceContext> g_ImmediateContext = nullptr;
	ComPtr<IDXGISwapChain> g_SwapChain = nullptr;

	// deferred rendering
	ComPtr<ID3D11DeviceContext> g_DeferredContext[JSH_GFX_COMMANDLISTS_COUNT];
	ComPtr<ID3D11CommandList> g_CommandLists[JSH_GFX_COMMANDLISTS_COUNT];

	jsh::safe_queue<jsh::CommandList, JSH_GFX_COMMANDLISTS_COUNT> g_ActiveCommandLists;
	jsh::safe_queue<jsh::CommandList, JSH_GFX_COMMANDLISTS_COUNT> g_FreeCommandLists;

	///////////////////CONVERSIONS////////////////////

	constexpr D3D11_USAGE ParseUsage(const JSH_USAGE usage)
	{
		switch (usage) {
		case JSH_USAGE_DEFAULT:
			return D3D11_USAGE_DEFAULT;
		case JSH_USAGE_DYNAMIC:
			return D3D11_USAGE_DYNAMIC;
		case JSH_USAGE_IMMUTABLE:
			return D3D11_USAGE_IMMUTABLE;
		case JSH_USAGE_STAGING:
			return D3D11_USAGE_STAGING;
		default:
			return D3D11_USAGE_DEFAULT;
		}
	}

	constexpr D3D11_PRIMITIVE_TOPOLOGY ParseTopology(const JSH_TOPOLOGY topology)
	{
		switch (topology) {
		case JSH_TOPOLOGY_LINES:
			return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case JSH_TOPOLOGY_LINE_STRIP:
			return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case JSH_TOPOLOGY_POINTS:
			return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		case JSH_TOPOLOGY_TRIANGLES:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case JSH_TOPOLOGY_TRIANGLE_STRIP:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		default:
			jshLogE("Invalid topology");
			return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}
	}

	constexpr D3D11_BIND_FLAG ParseBindFlag(const JSH_BUFFER_TYPE bufferType)
	{
		switch (bufferType) {
		case JSH_BUFFER_TYPE_VERTEX:
			return D3D11_BIND_VERTEX_BUFFER;
		case JSH_BUFFER_TYPE_INDEX:
			return D3D11_BIND_INDEX_BUFFER;
		case JSH_BUFFER_TYPE_CONSTANT:
			return D3D11_BIND_CONSTANT_BUFFER;
		case JSH_BUFFER_TYPE_NULL:
		default:
			jshLogE("Invalid topology");
			return D3D11_BIND_VERTEX_BUFFER;
		}
	}

	constexpr D3D11_FILTER ParseFilter(const JSH_FILTER filter)
	{
		return (D3D11_FILTER)filter;
	}
	constexpr D3D11_TEXTURE_ADDRESS_MODE ParseTextureAddress(const JSH_TEXTURE_ADDRESS_MODE addressMode)
	{
		return (D3D11_TEXTURE_ADDRESS_MODE)addressMode;
	}

	constexpr DXGI_FORMAT ParseFormat(const JSH_FORMAT format) {
		return (DXGI_FORMAT) format;
	}

	D3D11_INPUT_ELEMENT_DESC ParseInputElementDesc(const JSH_INPUT_ELEMENT_DESC& ied) 
	{
		D3D11_INPUT_ELEMENT_DESC dxIed;
		dxIed.AlignedByteOffset = ied.alignedByteOffset;
		dxIed.Format = ParseFormat(ied.format);
		dxIed.InputSlot = ied.inputSlot;
		dxIed.InputSlotClass = ied.perVertexData ? D3D11_INPUT_PER_VERTEX_DATA : D3D11_INPUT_PER_INSTANCE_DATA;
		dxIed.InstanceDataStepRate = ied.instanceDataStepRate;
		dxIed.SemanticIndex = ied.semanticIndex;
		dxIed.SemanticName = ied.semanticName;
		return dxIed;
	}

	constexpr D3D11_COMPARISON_FUNC ParseComparisonFunc(const JSH_COMPARISON_FUNC cf)
	{
		return (D3D11_COMPARISON_FUNC)cf;
	}

	constexpr D3D11_BLEND_OP ParseBlendOp(const JSH_BLEND_OP bo)
	{
		return (D3D11_BLEND_OP)bo;
	}
	constexpr D3D11_BLEND ParseBlend(const JSH_BLEND b)
	{
		return (D3D11_BLEND)b;
	}
	constexpr D3D11_STENCIL_OP ParseStencilOp(const JSH_STENCIL_OP so)
	{
		return (D3D11_STENCIL_OP)so;
	}
	constexpr D3D11_DEPTH_WRITE_MASK ParseDepthWriteMask(const JSH_DEPTH_WRITE_MASK dwm)
	{
		return (D3D11_DEPTH_WRITE_MASK)dwm;
	}
	constexpr D3D11_CULL_MODE ParseCullMode(const JSH_CULL_MODE cm)
	{
		return (D3D11_CULL_MODE)cm;
	}
	constexpr D3D11_FILL_MODE ParseFillMode(const JSH_FILL_MODE cm)
	{
		return (D3D11_FILL_MODE)cm;
	}
	constexpr D3D11_RTV_DIMENSION ParseRTVDimension(const JSH_RTV_DIMENSION rtv)
	{
		return (D3D11_RTV_DIMENSION)rtv;
	}

	bool Initialize() {

		HWND windowHandle = reinterpret_cast<HWND>(jshWindow::GetWindowHandle());

		DXGI_SWAP_CHAIN_DESC swapChainDescriptor;
		jshZeroMemory(&swapChainDescriptor, sizeof(DXGI_SWAP_CHAIN_DESC));
		swapChainDescriptor.BufferCount = 1;
		swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDescriptor.BufferDesc.Height = 0;
		swapChainDescriptor.BufferDesc.Width = 0;
		swapChainDescriptor.BufferDesc.RefreshRate.Denominator = 0;
		swapChainDescriptor.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDescriptor.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDescriptor.Flags = 0;
		swapChainDescriptor.OutputWindow = windowHandle;
		swapChainDescriptor.SampleDesc.Count = 1;
		swapChainDescriptor.SampleDesc.Quality = 0;
		swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDescriptor.Windowed = TRUE;

		jshGfx(D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			jshDebug::InDebugMode() ? D3D11_CREATE_DEVICE_DEBUG : 0,
			NULL,
			0,
			D3D11_SDK_VERSION,
			&swapChainDescriptor,
			&g_SwapChain,
			&g_Device,
			NULL,
			&g_ImmediateContext
		));

		// viewport
		D3D11_VIEWPORT viewport;
		viewport.Width = (float)jshWindow::GetWidth();
		viewport.Height = (float)jshWindow::GetHeight();
		viewport.MaxDepth = 1.f;
		viewport.MinDepth = 0.f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		g_ImmediateContext->RSSetViewports(1, &viewport);

		// multithreading support
		D3D11_FEATURE_DATA_THREADING data;
		g_Device->CheckFeatureSupport(D3D11_FEATURE_THREADING, &data, sizeof(D3D11_FEATURE_DATA_THREADING));
		jshLogI("%i, %i", data.DriverCommandLists, data.DriverConcurrentCreates);

		return true;
	}

#ifdef JSH_IMGUI
	bool InitializeImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		if (!ImGui_ImplWin32_Init(jshWindow::GetWindowHandle())) return false;
		if (!ImGui_ImplDX11_Init(g_Device.Get(), g_ImmediateContext.Get())) return false;
		return true;
	}
#endif

	bool Close()
	{
		return true;
	}

	void Begin()
	{
		
	}
	void End()
	{
		jsh::CommandList cmd;
		while (g_ActiveCommandLists.pop(cmd)) {

			g_DeferredContext[cmd]->FinishCommandList(FALSE, g_CommandLists[cmd].GetAddressOf());
			g_ImmediateContext->ExecuteCommandList(g_CommandLists[cmd].Get(), FALSE);
			g_CommandLists[cmd].Reset();

			g_FreeCommandLists.push(cmd);
		}
	}
	void Present()
	{
		g_SwapChain->Present(0u, 0u);
	}

#ifdef JSH_IMGUI
	void BeginImGui()
	{
		jshImGui(ImGui_ImplDX11_NewFrame());
		jshImGui(ImGui_ImplWin32_NewFrame());
		jshImGui(ImGui::NewFrame());
		jshImGui(jshDebug::ShowImGuiWindow());
	}
	void EndImGui(const jsh::RenderTargetView& rtv)
	{
		RenderTargetView_dx11* RTV = ToInternal(rtv);
		g_ImmediateContext->OMSetRenderTargets(1, RTV->ptr.GetAddressOf(), nullptr);

		jshImGui(ImGui::Render());
		jshImGui(ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()));
	}
#endif

	jsh::CommandList BeginCommandList()
	{
		jsh::CommandList cmd;
		if (!g_FreeCommandLists.pop(cmd)) {

			cmd = g_ActiveCommandLists.size();
			assert(cmd < JSH_GFX_COMMANDLISTS_COUNT);

			g_Device->CreateDeferredContext(0u, &g_DeferredContext[cmd]);
		}
		g_ActiveCommandLists.push(cmd);
		
		return cmd;
	}

	/////////////////////////TOPOLOGY//////////////////////
	void SetTopology(JSH_TOPOLOGY topology, jsh::CommandList cmd) {
		g_DeferredContext[cmd]->IASetPrimitiveTopology(ParseTopology(topology));
	}

	//////////////////////////////BUFFER//////////////////////////////
	void CreateBuffer(const JSH_BUFFER_DESC* d, JSH_SUBRESOURCE_DATA* s, jsh::Buffer* b)
	{
		auto buffer = std::make_shared<Buffer_dx11>();
		b->internalAllocation = buffer;

		D3D11_BUFFER_DESC desc;
		desc.BindFlags = d->BindFlags;
		desc.ByteWidth = d->ByteWidth;
		desc.CPUAccessFlags = d->CPUAccessFlags;
		desc.MiscFlags = d->MiscFlags;
		desc.StructureByteStride = d->StructureByteStride;
		desc.Usage = ParseUsage(d->Usage);

		D3D11_SUBRESOURCE_DATA subres;
		subres.pSysMem = s->pSysMem;
		subres.SysMemPitch = 0u;
		subres.SysMemSlicePitch = 0u;

		buffer->structureByteStride = desc.StructureByteStride;

		jshGfx(g_Device->CreateBuffer(&desc, &subres, buffer->ptr.GetAddressOf()));
	}
	void BindVertexBuffer(const jsh::Buffer& b, uint32 slot, jsh::CommandList cmd)
	{
		assert(b.IsValid());

		Buffer_dx11* buffer = ToInternal(b);

		const UINT strides = buffer->structureByteStride;
		const UINT offset = 0U;
		g_DeferredContext[cmd]->IASetVertexBuffers(slot, 1, buffer->ptr.GetAddressOf(), &strides, &offset);

		return;
	}
	void BindIndexBuffer(const jsh::Buffer& b, jsh::CommandList cmd)
	{
		assert(b.IsValid());

		Buffer_dx11* buffer = ToInternal(b);
		g_DeferredContext[cmd]->IASetIndexBuffer(buffer->ptr.Get(), DXGI_FORMAT_R32_UINT, 0);

		return;
	}
	void BindConstantBuffer(const jsh::Buffer& b, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd)
	{
		assert(b.IsValid());

		Buffer_dx11* buffer = ToInternal(b);

		switch (shaderType) {
		case JSH_SHADER_TYPE_VERTEX:
			g_DeferredContext[cmd]->VSSetConstantBuffers(slot, 1, buffer->ptr.GetAddressOf());
			break;
		case JSH_SHADER_TYPE_PIXEL:
			g_DeferredContext[cmd]->PSSetConstantBuffers(slot, 1, buffer->ptr.GetAddressOf());
			break;
		case JSH_SHADER_TYPE_GEOMETRY:
			g_DeferredContext[cmd]->GSSetConstantBuffers(slot, 1, buffer->ptr.GetAddressOf());
			break;
		case JSH_SHADER_TYPE_NULL:
		default:
			jshLogE("Invalid shader type");
			break;
		}

		return;
	}

	//////////////////////////////INPUT LAYOUT//////////////////////////////
	void CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* desc, uint32 cant, jsh::VertexShader& vs, jsh::InputLayout* il)
	{
		auto inputLayout = std::make_shared<InputLayout_dx11>();
		il->internalAllocation = inputLayout;

		jsh::vector<D3D11_INPUT_ELEMENT_DESC> elements;
		elements.reserve(cant);
		for (uint32 i = 0; i < cant; ++i) {
			elements[i] = ParseInputElementDesc(desc[i]);
		}

		VertexShader_dx11* vertexShader = ToInternal(vs);
		jshGfx(g_Device->CreateInputLayout(elements.data(), cant, vertexShader->blob->GetBufferPointer(), vertexShader->blob->GetBufferSize(), &inputLayout->ptr));
	}
	void BindInputLayout(const jsh::InputLayout& il, jsh::CommandList cmd)
	{
		assert(il.IsValid());
		InputLayout_dx11* inputLayout = ToInternal(il);
		g_DeferredContext[cmd]->IASetInputLayout(inputLayout->ptr.Get());
	}

	//////////////////////////////SHADERS//////////////////////////////
	void CreateVertexShader(const wchar* path, jsh::VertexShader* vs)
	{
		auto vertexShader = std::make_shared<VertexShader_dx11>();
		vs->internalAllocation = vertexShader;

		jshGfx(D3DReadFileToBlob(path, &vertexShader->blob));
		jshGfx(g_Device->CreateVertexShader(vertexShader->blob->GetBufferPointer(), vertexShader->blob->GetBufferSize(), nullptr, &vertexShader->ptr));
	}
	void CreatePixelShader(const wchar* path, jsh::PixelShader* ps)
	{
		auto pixelShader = std::make_shared<PixelShader_dx11>();
		ps->internalAllocation = pixelShader;

		ComPtr<ID3DBlob> blob;

		jshGfx(D3DReadFileToBlob(path, &blob));
		jshGfx(g_Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader->ptr));
	}
	void BindVertexShader(const jsh::VertexShader& vs, jsh::CommandList cmd)
	{
		assert(vs.IsValid());
		VertexShader_dx11* shader = ToInternal(vs);
		g_DeferredContext[cmd]->VSSetShader(shader->ptr.Get(), nullptr, 0);
	}
	void BindPixelShader(const jsh::PixelShader& ps, jsh::CommandList cmd)
	{
		assert(ps.IsValid());
		PixelShader_dx11* shader = ToInternal(ps);
		g_DeferredContext[cmd]->PSSetShader(shader->ptr.Get(), nullptr, 0);
	}

	//////////////////////////////TEXUTRE//////////////////////////////
	void CreateTexture(const JSH_TEXTURE2D_DESC* d, JSH_SUBRESOURCE_DATA* s, jsh::Texture* t)
	{
		auto texture = std::make_shared<Texture_dx11>();
		t->internalAllocation = texture;

		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize = d->ArraySize;
		texDesc.BindFlags = d->BindFlags;
		texDesc.CPUAccessFlags = d->CPUAccessFlags;
		texDesc.Format = ParseFormat(d->Format);
		texDesc.Width = d->Width;
		texDesc.Height = d->Height;
		texDesc.MipLevels = d->MipLevels;
		texDesc.MiscFlags = d->MiscFlags;
		texDesc.SampleDesc.Count = d->SampleDesc.Count;
		texDesc.SampleDesc.Quality = d->SampleDesc.Quality;
		texDesc.Usage = ParseUsage(d->Usage);

		if (s) {
			D3D11_SUBRESOURCE_DATA subDesc = {};
			subDesc.pSysMem = s->pSysMem;
			subDesc.SysMemPitch = s->SysMemPitch;
			subDesc.SysMemSlicePitch = s->SysMemSlicePitch;
			jshGfx(g_Device->CreateTexture2D(&texDesc, &subDesc, &texture->texturePtr));
		}
		else {
			jshGfx(g_Device->CreateTexture2D(&texDesc, nullptr, &texture->texturePtr));
		}

		// create views
		if (texDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE) {
			D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			viewDesc.Format = ParseFormat(d->Format);
			viewDesc.Texture2D.MipLevels = 1u;
			viewDesc.Texture2D.MostDetailedMip = 0u;

			jshGfx(g_Device->CreateShaderResourceView(texture->texturePtr.Get(), &viewDesc, &texture->shaderSrcView));
		}
		if (texDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL) {
			D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
			jshZeroMemory(&viewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			viewDesc.Texture2D.MipSlice = 0u;
			viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

			jshGfx(g_Device->CreateDepthStencilView(texture->texturePtr.Get(), &viewDesc, &texture->depthStencilView));
		}
	}
	void BindTexture(const jsh::Texture& t, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd)
	{
		assert(t.IsValid());
		Texture_dx11* texture = ToInternal(t);

		switch (shaderType)
		{
		case JSH_SHADER_TYPE_VERTEX:
			g_DeferredContext[cmd]->VSSetShaderResources(slot, 1u, texture->shaderSrcView.GetAddressOf());
			break;
		case JSH_SHADER_TYPE_PIXEL:
			g_DeferredContext[cmd]->PSSetShaderResources(slot, 1u, texture->shaderSrcView.GetAddressOf());
			break;
		case JSH_SHADER_TYPE_GEOMETRY:
			g_DeferredContext[cmd]->GSSetShaderResources(slot, 1u, texture->shaderSrcView.GetAddressOf());
			break;
		case JSH_SHADER_TYPE_NULL:
		default:
			jshLogE("Invalid shader type");
			break;
		}
	}
	void BindTexture(const jsh::RenderTargetView& rtv, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd)
	{
		assert(rtv.IsValid());
		RenderTargetView_dx11* RTV = ToInternal(rtv);
		assert(RTV->shaderResView.Get() != nullptr);

		switch (shaderType)
		{
		case JSH_SHADER_TYPE_VERTEX:
			g_DeferredContext[cmd]->VSSetShaderResources(slot, 1u, RTV->shaderResView.GetAddressOf());
			break;
		case JSH_SHADER_TYPE_PIXEL:
			g_DeferredContext[cmd]->PSSetShaderResources(slot, 1u, RTV->shaderResView.GetAddressOf());
			break;
		case JSH_SHADER_TYPE_GEOMETRY:
			g_DeferredContext[cmd]->GSSetShaderResources(slot, 1u, RTV->shaderResView.GetAddressOf());
			break;
		case JSH_SHADER_TYPE_NULL:
		default:
			jshLogE("Invalid shader type");
			break;
		}
	}

	/////////////////////////VIEWPORT////////////////////////////////////////
	void CreateViewport(float x, float y, float width, float height, jsh::Viewport* vp)
	{
		auto viewport = std::make_shared<Viewport_dx11>();
		vp->internalAllocation = viewport;

		viewport->Width = width;
		viewport->Height = height;
		viewport->MaxDepth = 1.f;
		viewport->MinDepth = 0.f;
		viewport->TopLeftX = x;
		viewport->TopLeftY = y;
	}
	void BindViewport(const jsh::Viewport& vp, uint32 slot, jsh::CommandList cmd)
	{
		assert(vp.IsValid());
		Viewport_dx11* viewport = ToInternal(vp);
		g_DeferredContext[cmd]->RSSetViewports(1, viewport);
	}

	/////////////////////////SAMPLER STATE////////////////////////////////////////
	void CreateSamplerState(const JSH_SAMPLER_DESC* d, jsh::SamplerState* ss)
	{
		auto samplerState = std::make_shared<SamplerState_dx11>();
		ss->internalAllocation = samplerState;

		D3D11_SAMPLER_DESC desc;
		desc.AddressU = ParseTextureAddress(d->AddressU);
		desc.AddressV = ParseTextureAddress(d->AddressV);
		desc.AddressW = ParseTextureAddress(d->AddressW);
		desc.BorderColor[0] = d->BorderColor[0];
		desc.BorderColor[1] = d->BorderColor[1];
		desc.BorderColor[2] = d->BorderColor[2];
		desc.BorderColor[3] = d->BorderColor[3];
		desc.ComparisonFunc = ParseComparisonFunc(d->ComparisonFunc);
		desc.Filter = ParseFilter(d->Filter);
		desc.MaxAnisotropy = d->MaxAnisotropy;
		desc.MaxLOD = d->MaxLOD;
		desc.MinLOD = d->MinLOD;
		desc.MipLODBias = d->MipLODBias;

		jshGfx(g_Device->CreateSamplerState(&desc, &samplerState->ptr));
	}
	void BindSamplerState(const jsh::SamplerState& ss, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd)
	{
		assert(ss.IsValid());
		SamplerState_dx11* samplerState = ToInternal(ss);
		switch (shaderType) {
		case JSH_SHADER_TYPE_VERTEX:
			g_DeferredContext[cmd]->VSSetSamplers(slot, 1, samplerState->ptr.GetAddressOf());
			return;
		case JSH_SHADER_TYPE_PIXEL:
			g_DeferredContext[cmd]->PSSetSamplers(slot, 1, samplerState->ptr.GetAddressOf());
			return;
		case JSH_SHADER_TYPE_GEOMETRY:
			g_DeferredContext[cmd]->GSSetSamplers(slot, 1, samplerState->ptr.GetAddressOf());
			return;
		}
	}

	/////////////////////////BLEND STATE////////////////////////////////////////
	void CreateBlendState(const JSH_BLEND_DESC* d, jsh::BlendState* bs)
	{
		auto blendState = std::make_shared<BlendState_dx11>();
		bs->internalAllocation = blendState;

		D3D11_BLEND_DESC desc;
		desc.AlphaToCoverageEnable = d->AlphaToCoverageEnable;
		desc.IndependentBlendEnable = d->IndependentBlendEnable;
		for (uint8 i = 0; i < 8; ++i) {
			desc.RenderTarget[i].BlendEnable = d->RenderTarget[i].BlendEnable;
			desc.RenderTarget[i].BlendOp = ParseBlendOp(d->RenderTarget[i].BlendOp);
			desc.RenderTarget[i].BlendOpAlpha = ParseBlendOp(d->RenderTarget[i].BlendOpAlpha);
			desc.RenderTarget[i].DestBlend = ParseBlend(d->RenderTarget[i].DestBlend);
			desc.RenderTarget[i].DestBlendAlpha = ParseBlend(d->RenderTarget[i].DestBlendAlpha);
			desc.RenderTarget[i].RenderTargetWriteMask = d->RenderTarget[i].RenderTargetWriteMask;
			desc.RenderTarget[i].SrcBlend = ParseBlend(d->RenderTarget[i].SrcBlend);
			desc.RenderTarget[i].SrcBlendAlpha = ParseBlend(d->RenderTarget[i].SrcBlendAlpha);
		}

		jshGfx(g_Device->CreateBlendState(&desc, &blendState->ptr));
	}
	void BindBlendState(const jsh::BlendState& bs, jsh::CommandList cmd)
	{
		assert(bs.IsValid());
		BlendState_dx11* blendState = ToInternal(bs);
		g_DeferredContext[cmd]->OMSetBlendState(blendState->ptr.Get(), nullptr, 0xffffffff);
	}

	/////////////////////////DEPTHSTENCIL STATE////////////////////////////////////////
	void CreateDepthStencilState(const JSH_DEPTH_STENCIL_DESC* d, jsh::DepthStencilState* dss)
	{
		auto depthStencilState = std::make_shared<DepthStencilState_dx11>();
		dss->internalAllocation = depthStencilState;

		D3D11_DEPTH_STENCIL_DESC desc;
		desc.BackFace.StencilDepthFailOp = ParseStencilOp(d->BackFace.StencilDepthFailOp);
		desc.BackFace.StencilFailOp = ParseStencilOp(d->BackFace.StencilFailOp);
		desc.BackFace.StencilFunc = ParseComparisonFunc(d->BackFace.StencilFunc);
		desc.BackFace.StencilPassOp = ParseStencilOp(d->BackFace.StencilPassOp);
		desc.DepthEnable = d->DepthEnable;
		desc.DepthFunc = ParseComparisonFunc(d->DepthFunc);
		desc.DepthWriteMask = ParseDepthWriteMask(d->DepthWriteMask);
		desc.FrontFace.StencilDepthFailOp = ParseStencilOp(d->BackFace.StencilDepthFailOp);
		desc.FrontFace.StencilFailOp = ParseStencilOp(d->BackFace.StencilFailOp);
		desc.FrontFace.StencilFunc = ParseComparisonFunc(d->BackFace.StencilFunc);
		desc.FrontFace.StencilPassOp = ParseStencilOp(d->BackFace.StencilPassOp);
		desc.StencilEnable = d->StencilEnable;
		desc.StencilReadMask = d->StencilReadMask;
		desc.StencilWriteMask = d->StencilWriteMask;

		jshGfx(g_Device->CreateDepthStencilState(&desc, &depthStencilState->statePtr));
	}
	void BindDepthStencilState(const jsh::DepthStencilState& ds, jsh::CommandList cmd)
	{
		assert(ds.IsValid());
		DepthStencilState_dx11* dsState = ToInternal(ds);
		g_DeferredContext[cmd]->OMSetDepthStencilState(dsState->statePtr.Get(), 1u);
	}
	void ClearDepthStencilView(const jsh::Texture& tex, jsh::CommandList cmd)
	{
		Texture_dx11* texture = ToInternal(tex);
		assert(texture->depthStencilView.Get() != nullptr);
		g_DeferredContext[cmd]->ClearDepthStencilView(texture->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	/////////////////////////RASTERIZER STATE////////////////////////////////////////
	void CreateRasterizerState(const JSH_RASTERIZER_DESC* d, jsh::RasterizerState* rs)
	{
		auto rasterizerState = std::make_shared<RasterizerState_dx11>();
		rs->internalAllocation = rasterizerState;

		D3D11_RASTERIZER_DESC desc;
		desc.AntialiasedLineEnable = d->AntialiasedLineEnable;
		desc.CullMode = ParseCullMode(d->CullMode);
		desc.DepthBias = d->DepthBias;
		desc.DepthBiasClamp = d->DepthBiasClamp;
		desc.DepthClipEnable = d->DepthClipEnable;
		desc.FillMode = ParseFillMode(d->FillMode);
		desc.FrontCounterClockwise = d->FrontCounterClockwise;
		desc.MultisampleEnable = d->MultisampleEnable;
		desc.ScissorEnable = d->ScissorEnable;
		desc.SlopeScaledDepthBias = d->SlopeScaledDepthBias;

		jshGfx(g_Device->CreateRasterizerState(&desc, &rasterizerState->ptr));
	}
	void BindRasterizerState(const jsh::RasterizerState& rs, jsh::CommandList cmd)
	{
		assert(rs.IsValid());
		RasterizerState_dx11* rasterizerState = ToInternal(rs);
		g_DeferredContext[cmd]->RSSetState(rasterizerState->ptr.Get());
	}

	/////////////////////////RENDER TARGET VIEW////////////////////////
	void CreateRenderTargetView(const JSH_RENDER_TARGET_VIEW_DESC* d, const JSH_TEXTURE2D_DESC* td, jsh::RenderTargetView* rtv)
	{
		auto RTV = std::make_shared<RenderTargetView_dx11>();
		rtv->internalAllocation = RTV;

		// resource
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize = td->ArraySize;
		texDesc.BindFlags = td->BindFlags;
		texDesc.CPUAccessFlags = td->CPUAccessFlags;
		texDesc.Format = ParseFormat(td->Format);
		texDesc.Width = td->Width;
		texDesc.Height = td->Height;
		texDesc.MipLevels = td->MipLevels;
		texDesc.MiscFlags = td->MiscFlags;
		texDesc.SampleDesc.Count = td->SampleDesc.Count;
		texDesc.SampleDesc.Quality = td->SampleDesc.Quality;
		texDesc.Usage = ParseUsage(td->Usage);

		// RTV
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		desc.Format = ParseFormat(d->Format);
		desc.ViewDimension = ParseRTVDimension(d->ViewDimension);
		desc.Texture2D.MipSlice = d->Texture2D.MipSlice;

		jshGfx(g_Device->CreateTexture2D(&texDesc, nullptr, &RTV->resourcePtr));

		if (td->BindFlags & JSH_BIND_SHADER_RESOURCE) {
			D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
			jshZeroMemory(&srDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			srDesc.Format = texDesc.Format;
			srDesc.Texture2D.MipLevels = 1u;
			srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			jshGfx(g_Device->CreateShaderResourceView(RTV->resourcePtr.Get(), &srDesc, &RTV->shaderResView));
		}

		jshGfx(g_Device->CreateRenderTargetView(RTV->resourcePtr.Get(), &desc, &RTV->ptr));

	}
	void CreateRenderTargetViewFromBackBuffer(JSH_RENDER_TARGET_VIEW_DESC* d, jsh::RenderTargetView* rtv)
	{
		auto RTV = std::make_shared<RenderTargetView_dx11>();
		rtv->internalAllocation = RTV;

		// resource
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		desc.Format = ParseFormat(d->Format);
		desc.ViewDimension = ParseRTVDimension(d->ViewDimension);
		desc.Texture2D.MipSlice = d->Texture2D.MipSlice;

		jshGfx(g_SwapChain->GetBuffer(0u, __uuidof(ID3D11Resource), &RTV->resourcePtr));
		jshGfx(g_Device->CreateRenderTargetView(RTV->resourcePtr.Get(), &desc, &RTV->ptr));
	}
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, jsh::CommandList cmd)
	{
		assert(rtv.IsValid());
		RenderTargetView_dx11* RTV = ToInternal(rtv);
		g_DeferredContext[cmd]->OMSetRenderTargets(1u, RTV->ptr.GetAddressOf(), nullptr);
	}
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, const jsh::Texture& tex, jsh::CommandList cmd)
	{
		assert(rtv.IsValid() && tex.IsValid());
		RenderTargetView_dx11* RTV = ToInternal(rtv);
		Texture_dx11* texture = ToInternal(tex);
		assert(texture->depthStencilView.Get() != nullptr);
		g_DeferredContext[cmd]->OMSetRenderTargets(1u, RTV->ptr.GetAddressOf(), texture->depthStencilView.Get());
	}
	void ClearRenderTargetView(const jsh::RenderTargetView& rtv, jsh::CommandList cmd)
	{
		RenderTargetView_dx11* renderTargetView = ToInternal(rtv);
		const float clearColor[] = { 0.f,0.f,0.f,1.f };
		g_DeferredContext[cmd]->ClearRenderTargetView(renderTargetView->ptr.Get(), clearColor);
	}

	/////////////////////////METHODS////////////////////////////////////////

	void UpdateConstantBuffer(jsh::Buffer& b, void* data, jsh::CommandList cmd)
	{
		Buffer_dx11* buffer = ToInternal(b);
		g_DeferredContext[cmd]->UpdateSubresource(buffer->ptr.Get(), 0, nullptr, data, 0, 0);
	}
	///////////////////////////RENDER CALLS/////////////////////////
	void DrawIndexed(uint32 indicesCount, jsh::CommandList cmd)
	{
		g_DeferredContext[cmd]->DrawIndexed(indicesCount, 0, 0);
	}

}
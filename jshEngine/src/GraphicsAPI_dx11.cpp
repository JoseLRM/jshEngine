#include "common.h"

#include "GraphicsAPI_dx11.h"

#include "WinLib.h"
#include "Window.h"
#include "Graphics.h"
#include "DirectX11Lib.h"

#include "safe_queue.h"

#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

namespace jshGraphics_dx11 {

	//////////////////GRAPHICS PRIMITIVES////////////////////

	struct Buffer_dx11 : public jshGraphics::_internal::Buffer_Internal {
		ComPtr<ID3D11Buffer> ptr;
		void Release() override
		{
			ptr.Reset();
		}
	};

	struct InputLayout_dx11 : public jshGraphics::_internal::InputLayout_Internal {
		ComPtr<ID3D11InputLayout> ptr;
		void Release() override
		{
			ptr.Reset();
		}
	};

	struct VertexShader_dx11 : public jshGraphics::_internal::VertexShader_Internal {
		ComPtr<ID3D11VertexShader> ptr;
		ComPtr<ID3DBlob> blob;
		void Release() override
		{
			ptr.Reset();
			blob.Reset();
		}
	};

	struct PixelShader_dx11 : public jshGraphics::_internal::PixelShader_Internal {
		ComPtr<ID3D11PixelShader> ptr;
		void Release() override
		{
			ptr.Reset();
		}
	};

	struct Texture_dx11 : public jshGraphics::_internal::TextureRes_Internal {
		ComPtr<ID3D11Texture2D> texturePtr;
		ComPtr<ID3D11ShaderResourceView> shaderSrcView;
		ComPtr<ID3D11DepthStencilView> depthStencilView;
		void Release() override
		{
			texturePtr.Reset();
			shaderSrcView.Reset();
			depthStencilView.Reset();
		}
	};

	struct Viewport_dx11 : public jshGraphics::_internal::Viewport_Internal {
		D3D11_VIEWPORT viewport;
		void Release() override {}
	};

	struct SamplerState_dx11 : public jshGraphics::_internal::SamplerState_Internal {
		ComPtr<ID3D11SamplerState> ptr;
		void Release() override
		{
			ptr.Reset();
		}
	};

	struct BlendState_dx11 : public jshGraphics::_internal::BlendState_Internal {
		ComPtr<ID3D11BlendState> ptr;
		void Release() override
		{
			ptr.Reset();
		}
	};

	struct DepthStencilState_dx11 : public jshGraphics::_internal::DepthStencilState_Internal {
		ComPtr<ID3D11DepthStencilState> statePtr;
		void Release() override
		{
			statePtr.Reset();
		}
	};

	struct RasterizerState_dx11 : public jshGraphics::_internal::RasterizerState_Internal {
		ComPtr<ID3D11RasterizerState> ptr;
		void Release() override
		{
			ptr.Reset();
		}
	};

	struct RenderTargetView_dx11 : public jshGraphics::_internal::RenderTargetView_Internal {
		ComPtr<ID3D11RenderTargetView> ptr;
		ComPtr<ID3D11Texture2D> resourcePtr;
		ComPtr<ID3D11ShaderResourceView> shaderResView;
		void Release() override
		{
			ptr.Reset();
			resourcePtr.Reset();
			shaderResView.Reset();
		}
	};

	inline Buffer_dx11* ToInternal(const jsh::Buffer& primitive)
	{ 
		return (Buffer_dx11*)primitive.internalAllocation; 
	}
	inline Texture_dx11* ToInternal(const jsh::TextureRes& primitive)
	{
		return (Texture_dx11*)primitive.internalAllocation;
	}
	inline InputLayout_dx11* ToInternal(const jsh::InputLayout& primitive)
	{
		return (InputLayout_dx11*)primitive.internalAllocation;
	}
	inline VertexShader_dx11* ToInternal(const jsh::VertexShader& primitive)
	{
		return (VertexShader_dx11*)primitive.internalAllocation;
	}
	inline PixelShader_dx11* ToInternal(const jsh::PixelShader& primitive)
	{
		return (PixelShader_dx11*)primitive.internalAllocation;
	}
	inline Viewport_dx11* ToInternal(const jsh::Viewport& primitive)
	{
		return (Viewport_dx11*)primitive.internalAllocation;
	}
	inline SamplerState_dx11* ToInternal(const jsh::SamplerState& primitive)
	{
		return (SamplerState_dx11*)primitive.internalAllocation;
	}
	inline BlendState_dx11* ToInternal(const jsh::BlendState& primitive)
	{
		return (BlendState_dx11*)primitive.internalAllocation;
	}
	inline DepthStencilState_dx11* ToInternal(const jsh::DepthStencilState& primitive)
	{
		return (DepthStencilState_dx11*)primitive.internalAllocation;
	}
	inline RasterizerState_dx11* ToInternal(const jsh::RasterizerState& primitive)
	{
		return (RasterizerState_dx11*)primitive.internalAllocation;
	}
	inline RenderTargetView_dx11* ToInternal(const jsh::RenderTargetView& primitive)
	{
		return (RenderTargetView_dx11*)primitive.internalAllocation;
	}

	////////////////////ALLOCATION//////////////////////////

	ComPtr<ID3D11Device> g_Device = nullptr;
	ComPtr<ID3D11DeviceContext> g_ImmediateContext = nullptr;

	ComPtr<IDXGISwapChain> g_SwapChain = nullptr;
	jsh::RenderTargetView g_RenderTargetView;

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
			jshDebug::LogE("Invalid topology");
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
			jshDebug::LogE("Invalid topology");
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
	constexpr DXGI_MODE_SCALING ParseScaling(const JSH_MODE_SCALING s)
	{
		return (DXGI_MODE_SCALING)s;
	}
	constexpr DXGI_MODE_SCANLINE_ORDER ParseScanlineOrder(const JSH_MODE_SCANLINE_ORDER s)
	{
		return (DXGI_MODE_SCANLINE_ORDER)s;
	}

	void CreateBackBuffer(DXGI_FORMAT format)
	{
		RenderTargetView_dx11* RTV = new RenderTargetView_dx11();
		g_RenderTargetView.internalAllocation = RTV;

		D3D11_RENDER_TARGET_VIEW_DESC desc;
		desc.Format = format;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0u;
		
		jshGfxFatal(g_SwapChain->GetBuffer(0u, __uuidof(ID3D11Resource), &RTV->resourcePtr));
		jshGfxFatal(g_Device->CreateRenderTargetView(RTV->resourcePtr.Get(), &desc, &RTV->ptr));
	}

	bool Initialize() {

		// load adapters
		{
			ComPtr<IDXGIFactory1> factory;
			jshGfxFatal(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)& factory));

			IDXGIAdapter1* adapter;
			for (uint32 i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {

				IDXGIOutput* output;
				for (uint32 j = 0; adapter->EnumOutputs(j, &output) != DXGI_ERROR_NOT_FOUND; ++j) {

					jsh::GraphicsAdapter jshAdapter;
					uint32 modesCount = 0u;
					DXGI_MODE_DESC* desc;

					jshAdapter.ptr = adapter;

					output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0u, &modesCount, nullptr);
					desc = new DXGI_MODE_DESC[modesCount];
					output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0u, &modesCount, desc);

					for (uint32 w = 0; w < modesCount; ++w) {
						DXGI_MODE_DESC& d = desc[w];

						jsh::OutputMode mode;

						mode.format = (JSH_FORMAT)d.Format;
						mode.resolution.x = d.Width;
						mode.resolution.y = d.Height;
						mode.refreshRateNumerator = d.RefreshRate.Numerator;
						mode.refreshRateDenominator = d.RefreshRate.Denominator;
						mode.scaling = (JSH_MODE_SCALING)d.Scaling;
						mode.scanlineOrdering = (JSH_MODE_SCANLINE_ORDER)d.ScanlineOrdering;
						mode.ptr = output;

						jshAdapter.AddMode(mode);
					}

					jshGraphics::_internal::AddAdapter(jshAdapter);
					delete[] desc;
				}

			}
		}

		// Set default output mode
		const jsh::OutputMode& outputMode = jshGraphics::GetAdapters().back().GetModes().back();
		jshGraphics::SetOutputMode(outputMode);

		// Create swap chain
		HWND windowHandle = reinterpret_cast<HWND>(jshWindow::GetWindowHandle());

		DXGI_SWAP_CHAIN_DESC swapChainDescriptor;
		jshZeroMemory(&swapChainDescriptor, sizeof(DXGI_SWAP_CHAIN_DESC));
		swapChainDescriptor.BufferCount = 1;
		swapChainDescriptor.BufferDesc.Format = ParseFormat(outputMode.format);
		swapChainDescriptor.BufferDesc.Height = outputMode.resolution.y;
		swapChainDescriptor.BufferDesc.Width = outputMode.resolution.x;
		swapChainDescriptor.BufferDesc.RefreshRate.Denominator = outputMode.refreshRateDenominator;
		swapChainDescriptor.BufferDesc.RefreshRate.Numerator = outputMode.refreshRateNumerator;
		swapChainDescriptor.BufferDesc.Scaling = ParseScaling(outputMode.scaling);
		swapChainDescriptor.BufferDesc.ScanlineOrdering = ParseScanlineOrder(outputMode.scanlineOrdering);
		swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDescriptor.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDescriptor.OutputWindow = windowHandle;
		swapChainDescriptor.SampleDesc.Count = 1;
		swapChainDescriptor.SampleDesc.Quality = 0;
		swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDescriptor.Windowed = TRUE;

		jshGfxFatal(D3D11CreateDeviceAndSwapChain(
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

		CreateBackBuffer(ParseFormat(outputMode.format));

		// multithreading support
		/*
		D3D11_FEATURE_DATA_THREADING data;
		g_Device->CheckFeatureSupport(D3D11_FEATURE_THREADING, &data, sizeof(D3D11_FEATURE_DATA_THREADING));
		jshDebug::LogI("%i, %i", data.DriverCommandLists, data.DriverConcurrentCreates);
		*/

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
		g_ImmediateContext.Reset();
		g_SwapChain.Reset();
		g_Device.Reset();
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
	void Present(uint32 interval)
	{
		g_SwapChain->Present(interval, 0u);
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

			cmd = jsh::CommandList(g_ActiveCommandLists.size());
			JSH_ASSERT(cmd < JSH_GFX_COMMANDLISTS_COUNT);

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
		Buffer_dx11* buffer = new Buffer_dx11();
		b->internalAllocation = buffer;

		D3D11_BUFFER_DESC desc;
		desc.BindFlags = d->BindFlags;
		desc.ByteWidth = d->ByteWidth;
		desc.CPUAccessFlags = d->CPUAccessFlags;
		desc.MiscFlags = d->MiscFlags;
		desc.StructureByteStride = d->StructureByteStride;
		desc.Usage = ParseUsage(d->Usage);

		D3D11_SUBRESOURCE_DATA subres;
		if (s != nullptr) {
			subres.pSysMem = s->pSysMem;
			subres.SysMemPitch = 0u;
			subres.SysMemSlicePitch = 0u;
		}

		jshGfx(g_Device->CreateBuffer(&desc, (s == nullptr) ? nullptr : &subres, buffer->ptr.GetAddressOf()));
	}
	void BindVertexBuffers(const jsh::Buffer* b, uint32 slot, uint32 count, const uint32* strides, const uint32* offsets, jsh::CommandList cmd)
	{
		ID3D11Buffer* buffers[JSH_GFX_VERTEX_BUFFERS_COUNT];

		for(uint32 i = 0; i < count; ++i) {
			buffers[i] = ToInternal(b[i])->ptr.Get();
		}

		g_DeferredContext[cmd]->IASetVertexBuffers(slot, count, buffers, strides, offsets);

		return;
	}
	void BindIndexBuffer(const jsh::Buffer& b, JSH_FORMAT format, uint32 offset, jsh::CommandList cmd)
	{
		JSH_ASSERT(b.IsValid());

		Buffer_dx11* buffer = ToInternal(b);
		g_DeferredContext[cmd]->IASetIndexBuffer(buffer->ptr.Get(), ParseFormat(format), offset);

		return;
	}
	void BindConstantBuffers(const jsh::Buffer* b, uint32 slot, uint32 count, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd)
	{
		ID3D11Buffer* buffers[JSH_GFX_CONSTANT_BUFFERS_COUNT];
		for (uint32 i = 0; i < count; ++i) {
			buffers[i] = ToInternal(b[i])->ptr.Get();
		}

		switch (shaderType) {
		case JSH_SHADER_TYPE_VERTEX:
			g_DeferredContext[cmd]->VSSetConstantBuffers(slot, count, buffers);
			break;
		case JSH_SHADER_TYPE_PIXEL:
			g_DeferredContext[cmd]->PSSetConstantBuffers(slot, count, buffers);
			break;
		case JSH_SHADER_TYPE_GEOMETRY:
			g_DeferredContext[cmd]->GSSetConstantBuffers(slot, count, buffers);
			break;
		case JSH_SHADER_TYPE_NULL:
		default:
			jshDebug::LogE("Invalid shader type");
			break;
		}

		return;
	}

	void UpdateBuffer(jsh::Buffer& b, void* data, uint32 size, jsh::CommandList cmd)
	{
		Buffer_dx11* buffer = ToInternal(b);
		JSH_ASSERT(buffer->desc.Usage != JSH_USAGE_IMMUTABLE && buffer->desc.ByteWidth >= size);

		if (buffer->desc.Usage == JSH_USAGE_DYNAMIC) {
			D3D11_MAPPED_SUBRESOURCE map;
			g_DeferredContext[cmd]->Map(buffer->ptr.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &map);

			memcpy(map.pData, data, (size == 0u) ? buffer->desc.ByteWidth : size);

			g_DeferredContext[cmd]->Unmap(buffer->ptr.Get(), 0u);
		}
		else {
			g_DeferredContext[cmd]->UpdateSubresource(buffer->ptr.Get(), 0, nullptr, data, 0, 0);
		}
	}

	//////////////////////////////INPUT LAYOUT//////////////////////////////
	void CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* desc, uint32 cant, jsh::VertexShader& vs, jsh::InputLayout* il)
	{
		InputLayout_dx11* inputLayout = new InputLayout_dx11();
		il->internalAllocation = inputLayout;

		D3D11_INPUT_ELEMENT_DESC elements[JSH_GFX_INPUT_LAYOUT_COUNT];
		for (uint32 i = 0; i < cant; ++i) {
			elements[i] = ParseInputElementDesc(desc[i]);
		}

		VertexShader_dx11* vertexShader = ToInternal(vs);
		jshGfx(g_Device->CreateInputLayout(elements, cant, vertexShader->blob->GetBufferPointer(), vertexShader->blob->GetBufferSize(), &inputLayout->ptr));
	}
	void BindInputLayout(const jsh::InputLayout& il, jsh::CommandList cmd)
	{
		JSH_ASSERT(il.IsValid());
		InputLayout_dx11* inputLayout = ToInternal(il);
		g_DeferredContext[cmd]->IASetInputLayout(inputLayout->ptr.Get());
	}

	//////////////////////////////SHADERS//////////////////////////////
	void CreateVertexShader(const wchar* path, jsh::VertexShader* vs)
	{
		VertexShader_dx11* vertexShader = new VertexShader_dx11();
		vs->internalAllocation = vertexShader;

		jshGfx(D3DReadFileToBlob(path, &vertexShader->blob));
		jshGfx(g_Device->CreateVertexShader(vertexShader->blob->GetBufferPointer(), vertexShader->blob->GetBufferSize(), nullptr, &vertexShader->ptr));
	}
	void CreatePixelShader(const wchar* path, jsh::PixelShader* ps)
	{
		PixelShader_dx11* pixelShader = new PixelShader_dx11();
		ps->internalAllocation = pixelShader;

		ComPtr<ID3DBlob> blob;

		jshGfx(D3DReadFileToBlob(path, &blob));
		jshGfx(g_Device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader->ptr));
	}
	void BindVertexShader(const jsh::VertexShader& vs, jsh::CommandList cmd)
	{
		JSH_ASSERT(vs.IsValid());
		VertexShader_dx11* shader = ToInternal(vs);
		g_DeferredContext[cmd]->VSSetShader(shader->ptr.Get(), nullptr, 0);
	}
	void BindPixelShader(const jsh::PixelShader& ps, jsh::CommandList cmd)
	{
		JSH_ASSERT(ps.IsValid());
		PixelShader_dx11* shader = ToInternal(ps);
		g_DeferredContext[cmd]->PSSetShader(shader->ptr.Get(), nullptr, 0);
	}

	//////////////////////////////TEXUTRE//////////////////////////////
	void CreateTextureRes(const JSH_TEXTURE2D_DESC* d, JSH_SUBRESOURCE_DATA* s, jsh::TextureRes* t)
	{
		Texture_dx11* texture = new Texture_dx11();
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
	void BindTexture(const jsh::TextureRes& t, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd)
	{
		JSH_ASSERT(t.IsValid());
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
			jshDebug::LogE("Invalid shader type");
			break;
		}
	}
	void BindTexture(const jsh::RenderTargetView& rtv, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd)
	{
		JSH_ASSERT(rtv.IsValid());
		RenderTargetView_dx11* RTV = ToInternal(rtv);
		JSH_ASSERT(RTV->shaderResView.Get() != nullptr);

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
			jshDebug::LogE("Invalid shader type");
			break;
		}
	}
	void UnbindTexture(uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd)
	{
		ID3D11ShaderResourceView* texture = nullptr;

		switch (shaderType)
		{
		case JSH_SHADER_TYPE_VERTEX:
			g_DeferredContext[cmd]->VSSetShaderResources(slot, 1u, &texture);
			break;
		case JSH_SHADER_TYPE_PIXEL:
			g_DeferredContext[cmd]->PSSetShaderResources(slot, 1u, &texture);
			break;
		case JSH_SHADER_TYPE_GEOMETRY:
			g_DeferredContext[cmd]->GSSetShaderResources(slot, 1u, &texture);
			break;
		case JSH_SHADER_TYPE_NULL:
		default:
			jshDebug::LogE("Invalid shader type");
			break;
		}
	}

	void UpdateTexture(jsh::TextureRes& res, void* data, uint32 size, jsh::CommandList cmd)
	{
		Texture_dx11* tex = ToInternal(res);
		uint32 texSize = tex->desc.Width * tex->desc.Height * jshGraphics::GetFormatStride(tex->desc.Format);
		JSH_ASSERT(tex->desc.Usage != JSH_USAGE_IMMUTABLE && texSize >= size);

		if (tex->desc.Usage == JSH_USAGE_DYNAMIC) {
			D3D11_MAPPED_SUBRESOURCE map;
			g_DeferredContext[cmd]->Map(tex->texturePtr.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &map);

			memcpy(map.pData, data, (size == 0u) ? texSize : size);

			g_DeferredContext[cmd]->Unmap(tex->texturePtr.Get(), 0u);
		}
		else {
			g_DeferredContext[cmd]->UpdateSubresource(tex->texturePtr.Get(), 0, nullptr, data, 0, 0);
		}
	}

	/////////////////////////VIEWPORT////////////////////////////////////////
	void CreateViewport(float x, float y, float width, float height, jsh::Viewport* vp)
	{
		Viewport_dx11* viewport = new Viewport_dx11();
		vp->internalAllocation = viewport;

		viewport->viewport.Width = width;
		viewport->viewport.Height = height;
		viewport->viewport.MaxDepth = 1.f;
		viewport->viewport.MinDepth = 0.f;
		viewport->viewport.TopLeftX = x;
		viewport->viewport.TopLeftY = y;
	}
	void BindViewport(const jsh::Viewport& vp, uint32 slot, jsh::CommandList cmd)
	{
		JSH_ASSERT(vp.IsValid());
		Viewport_dx11* viewport = ToInternal(vp);
		g_DeferredContext[cmd]->RSSetViewports(1, &viewport->viewport);
	}

	/////////////////////////SAMPLER STATE////////////////////////////////////////
	void CreateSamplerState(const JSH_SAMPLER_DESC* d, jsh::SamplerState* ss)
	{
		SamplerState_dx11* samplerState = new SamplerState_dx11();
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
		JSH_ASSERT(ss.IsValid());
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
		BlendState_dx11* blendState = new BlendState_dx11();
		bs->internalAllocation = blendState;

		D3D11_BLEND_DESC desc = {};
		desc.AlphaToCoverageEnable = d->AlphaToCoverageEnable;
		desc.IndependentBlendEnable = d->IndependentBlendEnable;
		for (uint8 i = 0; i < 1; ++i) {
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
		JSH_ASSERT(bs.IsValid());
		BlendState_dx11* blendState = ToInternal(bs);
		g_DeferredContext[cmd]->OMSetBlendState(blendState->ptr.Get(), nullptr, 0xffffffff);
	}

	/////////////////////////DEPTHSTENCIL STATE////////////////////////////////////////
	void CreateDepthStencilState(const JSH_DEPTH_STENCIL_DESC* d, jsh::DepthStencilState* dss)
	{
		DepthStencilState_dx11* depthStencilState = new DepthStencilState_dx11();
		dss->internalAllocation = depthStencilState;

		D3D11_DEPTH_STENCIL_DESC desc;
		desc.BackFace.StencilDepthFailOp = ParseStencilOp(d->BackFace.StencilDepthFailOp);
		desc.BackFace.StencilFailOp = ParseStencilOp(d->BackFace.StencilFailOp);
		desc.BackFace.StencilFunc = ParseComparisonFunc(d->BackFace.StencilFunc);
		desc.BackFace.StencilPassOp = ParseStencilOp(d->BackFace.StencilPassOp);
		desc.DepthEnable = d->DepthEnable;
		desc.DepthFunc = ParseComparisonFunc(d->DepthFunc);
		desc.DepthWriteMask = ParseDepthWriteMask(d->DepthWriteMask);
		desc.FrontFace.StencilDepthFailOp = ParseStencilOp(d->FrontFace.StencilDepthFailOp);
		desc.FrontFace.StencilFailOp = ParseStencilOp(d->FrontFace.StencilFailOp);
		desc.FrontFace.StencilFunc = ParseComparisonFunc(d->FrontFace.StencilFunc);
		desc.FrontFace.StencilPassOp = ParseStencilOp(d->FrontFace.StencilPassOp);
		desc.StencilEnable = d->StencilEnable;
		desc.StencilReadMask = d->StencilReadMask;
		desc.StencilWriteMask = d->StencilWriteMask;

		jshGfx(g_Device->CreateDepthStencilState(&desc, &depthStencilState->statePtr));
	}
	void BindDepthStencilState(const jsh::DepthStencilState& ds, uint32 stencilRef, jsh::CommandList cmd)
	{
		JSH_ASSERT(ds.IsValid());
		DepthStencilState_dx11* dsState = ToInternal(ds);
		g_DeferredContext[cmd]->OMSetDepthStencilState(dsState->statePtr.Get(), stencilRef);
	}
	void ClearDepthStencilView(const jsh::TextureRes& tex, jsh::CommandList cmd)
	{
		Texture_dx11* texture = ToInternal(tex);
		JSH_ASSERT(texture->depthStencilView.Get() != nullptr);
		g_DeferredContext[cmd]->ClearDepthStencilView(texture->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0u);
	}

	/////////////////////////RASTERIZER STATE////////////////////////////////////////
	void CreateRasterizerState(const JSH_RASTERIZER_DESC* d, jsh::RasterizerState* rs)
	{
		RasterizerState_dx11* rasterizerState = new RasterizerState_dx11();
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
		JSH_ASSERT(rs.IsValid());
		RasterizerState_dx11* rasterizerState = ToInternal(rs);
		g_DeferredContext[cmd]->RSSetState(rasterizerState->ptr.Get());
	}

	/////////////////////////RENDER TARGET VIEW////////////////////////
	void CreateRenderTargetView(const JSH_RENDER_TARGET_VIEW_DESC* d, const JSH_TEXTURE2D_DESC* td, jsh::RenderTargetView* rtv)
	{
		RenderTargetView_dx11* RTV = new RenderTargetView_dx11();
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
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, jsh::CommandList cmd)
	{
		JSH_ASSERT(rtv.IsValid());
		RenderTargetView_dx11* RTV = ToInternal(rtv);
		g_DeferredContext[cmd]->OMSetRenderTargets(1u, RTV->ptr.GetAddressOf(), nullptr);
	}
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, const jsh::TextureRes& tex, jsh::CommandList cmd)
	{
		JSH_ASSERT(rtv.IsValid() && tex.IsValid());
		RenderTargetView_dx11* RTV = ToInternal(rtv);
		Texture_dx11* texture = ToInternal(tex);
		JSH_ASSERT(texture->depthStencilView.Get() != nullptr);
		g_DeferredContext[cmd]->OMSetRenderTargets(1u, RTV->ptr.GetAddressOf(), texture->depthStencilView.Get());
	}
	void ClearRenderTargetView(const jsh::RenderTargetView& rtv, float r, float g, float b, float a, jsh::CommandList cmd)
	{
		RenderTargetView_dx11* renderTargetView = ToInternal(rtv);
		const float clearColor[] = { r, g, b, a };
		g_DeferredContext[cmd]->ClearRenderTargetView(renderTargetView->ptr.Get(), clearColor);
	}
	jsh::RenderTargetView& GetRenderTargetViewFromBackBuffer()
	{
		return g_RenderTargetView;
	}

	///////////////////////////RENDER CALLS/////////////////////////
	void DrawIndexed(uint32 indicesCount, jsh::CommandList cmd)
	{
		g_DeferredContext[cmd]->DrawIndexed(indicesCount, 0, 0);
	}
	void Draw(uint32 vertexCount, jsh::CommandList cmd)
	{
		g_DeferredContext[cmd]->Draw(vertexCount, 0);
	}
	void DrawInstanced(uint32 vertexPerInstance, uint32 instances, uint32 startVertex, uint32 startInstance, jsh::CommandList cmd)
	{
		g_DeferredContext[cmd]->DrawInstanced(vertexPerInstance, instances, startVertex, startInstance);
	}
	void DrawIndexedInstanced(uint32 indexPerInstance, uint32 instances, uint32 startIndex, uint32 startVertex, uint32 startInstance, jsh::CommandList cmd)
	{
		g_DeferredContext[cmd]->DrawIndexedInstanced(indexPerInstance, instances, startIndex, startVertex, startInstance);
	}

	void SetOutputMode(jsh::OutputMode mode)
	{
		RenderTargetView_dx11* rtv = ToInternal(g_RenderTargetView);

		rtv->ptr.Reset();
		rtv->resourcePtr.Reset();
		
		jshGfxFatal(g_SwapChain->ResizeBuffers(1u, mode.resolution.x, mode.resolution.y, ParseFormat(mode.format), DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

		CreateBackBuffer(ParseFormat(mode.format));
	}
	void SetFullscreen(bool fullscreen, const jsh::OutputMode& outputMode)
	{
		g_SwapChain->SetFullscreenState(BOOL(fullscreen), fullscreen ? reinterpret_cast<IDXGIOutput*>(outputMode.ptr) : nullptr);
	}

}
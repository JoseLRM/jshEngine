#include "GraphicsAPI_dx11.h"

#include "..//WinLib.h"
#include "..//Window.h"
#include "DirectX11Lib.h"

#include "..//..//utils/dataStructures/memory_pool.h"
#include "..//..//Debug.h"

#include "..//..//ImGui/imgui.h"
#include "..//..//ImGui/imgui_impl_win32.h"
#include "..//..//ImGui/imgui_impl_dx11.h"

namespace jshGraphics_dx11 {

	//////////////////GRAPHICS PRIMITIVES////////////////////

	struct Buffer {
		ID3D11Buffer* ptr = nullptr;
		void* dataPtr;
		uint32 structureByteStride;
	};

	struct VertexShader {
		ID3D11VertexShader* ptr = nullptr;
		ID3DBlob* blob;
	};

	struct PixelShader {
		ID3D11PixelShader* ptr = nullptr;
	};

	struct InputLayout {
		jsh::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
		ID3D11InputLayout* ptr = nullptr;
	};

	struct Texture {
		ID3D11Texture2D* texturePtr;
		ID3D11ShaderResourceView* viewPtr;
		ID3D11SamplerState* samplerStatePtr;
	};

	struct FrameBuffer {
		// render target
		ID3D11RenderTargetView* targetPtr;
		ID3D11Texture2D* targetTexturePtr;
		// depthstencil state
		uint8 state; // 0 -> all disable, 1 -> depth, 2 -> stencil, 3 -> all
		ID3D11DepthStencilState* dsStatePtr;
		ID3D11Texture2D* dsTexturePtr;
		ID3D11DepthStencilView* dsViewPtr;
	};

	////////////////////ALLOCATION//////////////////////////

	jsh::memory_pool<Buffer> g_Buffers;
	jsh::memory_pool<InputLayout> g_InputLayouts;

	jsh::memory_pool<VertexShader> g_VertexShaders;
	jsh::memory_pool<PixelShader> g_PixelShaders;

	jsh::memory_pool<Texture> g_Textures;

	jsh::memory_pool<FrameBuffer> g_FrameBuffers;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	jsh::FrameBuffer g_MainFrameBuffer = jsh::INVALID_FRAME_BUFFER;

	///////////////////CONVERSIONS////////////////////

	constexpr D3D11_USAGE ParseUsage(JSH_USAGE usage)
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

	constexpr D3D11_PRIMITIVE_TOPOLOGY ParseTopology(JSH_TOPOLOGY topology)
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

	constexpr D3D11_BIND_FLAG ParseBindFlag(JSH_BUFFER_TYPE bufferType)
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

	constexpr D3D11_FILTER ParseFilter(JSH_FILTER filter)
	{
		return (D3D11_FILTER)filter;
	}
	constexpr D3D11_TEXTURE_ADDRESS_MODE ParseTextureAddress(JSH_TEXTURE_ADDRESS_MODE addressMode)
	{
		return (D3D11_TEXTURE_ADDRESS_MODE)addressMode;
	}

	constexpr DXGI_FORMAT ParseFormat(JSH_FORMAT format) {
		return (DXGI_FORMAT) format;
	}

	D3D11_INPUT_ELEMENT_DESC ParseInputElementDesc(JSH_INPUT_ELEMENT_DESC ied) 
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

	bool Initialize() {

		HWND windowHandle = reinterpret_cast<HWND>(jshWindow::GetWindowHandle());

		DXGI_SWAP_CHAIN_DESC swapChainDescriptor = {};
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

		D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			jshDebug::InDebugMode() ? D3D11_CREATE_DEVICE_DEBUG : 0,
			NULL,
			0,
			D3D11_SDK_VERSION,
			&swapChainDescriptor,
			&swapChain,
			&device,
			NULL,
			&context
		);

		// getting swap chain back buffer and creating the render target view
		g_MainFrameBuffer = g_FrameBuffers.push();
		FrameBuffer& frameBuffer = g_FrameBuffers[g_MainFrameBuffer];

		swapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&frameBuffer.targetTexturePtr));

		D3D11_RENDER_TARGET_VIEW_DESC targetDesc = {};
		targetDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		targetDesc.Texture2D.MipSlice = 0u;
		targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		if (frameBuffer.targetTexturePtr) device->CreateRenderTargetView(frameBuffer.targetTexturePtr, &targetDesc, &frameBuffer.targetPtr);
		else return false;

		frameBuffer.state = 1u;

		// main depthstencil state
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = FALSE;
		device->CreateDepthStencilState(&dsDesc, &frameBuffer.dsStatePtr);

		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.ArraySize = 1u;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0u;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.Width = 1080;
		texDesc.Height = 720;
		texDesc.SampleDesc.Count = 1u;
		texDesc.SampleDesc.Quality = 0u;
		texDesc.MipLevels = 1u;
		texDesc.MiscFlags = 0u;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		device->CreateTexture2D(&texDesc, nullptr, &frameBuffer.dsTexturePtr);

		D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc = {};
		dsViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsViewDesc.Texture2D.MipSlice = 0u;
		device->CreateDepthStencilView(frameBuffer.dsTexturePtr, &dsViewDesc, &frameBuffer.dsViewPtr);
			
		context->OMSetRenderTargets(1, &frameBuffer.targetPtr, frameBuffer.dsViewPtr);

		// default topology
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// viewport
		D3D11_VIEWPORT viewport;
		viewport.Width = (float)jshWindow::GetWidth();
		viewport.Height = (float)jshWindow::GetHeight();
		viewport.MaxDepth = 1.f;
		viewport.MinDepth = 0.f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		context->RSSetViewports(1, &viewport);

		return true;
	}

#ifdef JSH_IMGUI
	bool InitializeImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		if (!ImGui_ImplWin32_Init(jshWindow::GetWindowHandle())) return false;
		if (!ImGui_ImplDX11_Init(device, context)) return false;

		jshImGui(ImGui_ImplDX11_NewFrame());
		jshImGui(ImGui_ImplWin32_NewFrame());
		jshImGui(ImGui::NewFrame());
		jshImGui(jshDebug::ShowImGuiWindow());

		return true;
	}
#endif

	bool Close()
	{

		//clear buffers
		while (!g_Buffers.empty()) {
			Buffer& buffer = g_Buffers.pop();
			if (!buffer.ptr) continue;
			buffer.ptr->Release();
		}
		g_Buffers.clear();

		// clear shaders
		while (!g_VertexShaders.empty()) {
			VertexShader& vs = g_VertexShaders.pop();
			if (!vs.ptr) continue;
			vs.ptr->Release();
			vs.blob->Release();
		}
		g_VertexShaders.clear();

		while (!g_PixelShaders.empty()) {
			PixelShader& ps = g_PixelShaders.pop();
			if (!ps.ptr) continue;
			ps.ptr->Release();
		}
		g_PixelShaders.clear();

		// clear input layout
		while (!g_InputLayouts.empty()) {
			InputLayout& il = g_InputLayouts.pop();
			if (!il.ptr) continue;
			il.ptr->Release();
		}
		g_InputLayouts.clear();

		// clear depthstencil states
		while (!g_FrameBuffers.empty()) {
			FrameBuffer& s = g_FrameBuffers.pop();
			if (!s.targetPtr) continue;
			s.dsStatePtr->Release();
			s.dsTexturePtr->Release();
			s.dsViewPtr->Release();
			s.targetPtr->Release();
			s.targetTexturePtr->Release();
		}
		g_FrameBuffers.clear();

		if (device)		device->Release();
		if (context)	context->Release();
		if (swapChain)	swapChain->Release();

		return true;
	}

	void Prepare(float* clearScreenColor)
	{
		jshImGui(ImGui::Render());
		jshImGui(ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()));

		// main frame buffer
		FrameBuffer& fb = g_FrameBuffers[g_MainFrameBuffer];
		swapChain->Present(1u, 0u);
		context->ClearRenderTargetView(fb.targetPtr, clearScreenColor);
		context->ClearDepthStencilView(fb.dsViewPtr, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0u);

		jshImGui(ImGui_ImplDX11_NewFrame());
		jshImGui(ImGui_ImplWin32_NewFrame());
		jshImGui(ImGui::NewFrame());
		jshImGui(jshDebug::ShowImGuiWindow());
	}

	void SetTopology(JSH_TOPOLOGY topology) {
		context->IASetPrimitiveTopology(ParseTopology(topology));
	}

	//////////////////////////////CREATION//////////////////////////////

	jsh::Buffer CreateBuffer(void* data, uint32 size, uint32 stride, JSH_USAGE usage, JSH_BUFFER_TYPE bufferType)
	{
		jsh::Buffer ID = g_Buffers.push();
		Buffer& buffer = g_Buffers[ID];

		D3D11_BUFFER_DESC desc;
		desc.BindFlags = ParseBindFlag(bufferType);
		desc.ByteWidth = size;
		desc.CPUAccessFlags = (usage == JSH_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.MiscFlags = 0u;
		desc.StructureByteStride = stride;
		desc.Usage = ParseUsage(usage);

		D3D11_SUBRESOURCE_DATA subres;
		subres.pSysMem = data;
		subres.SysMemPitch = 0u;
		subres.SysMemSlicePitch = 0u;

		buffer.dataPtr = data;
		buffer.structureByteStride = stride;

		device->CreateBuffer(&desc, &subres, &buffer.ptr);

		return ID;
	}

	jsh::InputLayout CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* desc, uint32 cant, jsh::VertexShader shaderID)
	{
		jsh::InputLayout ID = g_InputLayouts.push();
		InputLayout& inputLayout = g_InputLayouts[ID];

		inputLayout.inputElements.reserve(cant);
		inputLayout.inputElements.add_pos(cant);
		for (uint32 i = 0; i < cant; ++i) {
			inputLayout.inputElements[i] = ParseInputElementDesc(desc[i]);
		}

		VertexShader& vs = g_VertexShaders[shaderID];

		device->CreateInputLayout(inputLayout.inputElements.data(), cant, vs.blob->GetBufferPointer(), vs.blob->GetBufferSize(), &inputLayout.ptr);
		return ID;
	}

	jsh::VertexShader CreateVertexShader(const wchar* path)
	{
		jsh::VertexShader ID = g_VertexShaders.push();
		VertexShader& vs = g_VertexShaders[ID];

		D3DReadFileToBlob(path, &vs.blob);
		device->CreateVertexShader(vs.blob->GetBufferPointer(), vs.blob->GetBufferSize(), nullptr, &vs.ptr);
		return ID;
	}

	jsh::PixelShader CreatePixelShader(const wchar* path)
	{
		jsh::PixelShader ID = g_PixelShaders.push();
		PixelShader& ps = g_PixelShaders[ID];

		ID3DBlob* blob;

		D3DReadFileToBlob(path, &blob);
		device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &ps.ptr);
		if(blob) blob->Release();
		return ID;
	}

	jsh::FrameBuffer CreateFrameBuffer(uint32 width, uint32 height)
	{
		jsh::FrameBuffer ID = g_FrameBuffers.push();
		FrameBuffer& frameBuffer = g_FrameBuffers[ID];

		// render target
		CD3D11_TEXTURE2D_DESC targetTex;
		targetTex.Width = width;
		targetTex.Height = height;
		targetTex.ArraySize = 1u;
		targetTex.BindFlags = D3D11_BIND_RENDER_TARGET;
		targetTex.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		targetTex.MipLevels = 1u;
		targetTex.MiscFlags = 0u;
		targetTex.SampleDesc.Count = 1u;
		targetTex.SampleDesc.Quality = 0u;
		targetTex.CPUAccessFlags = 0u;
		targetTex.Usage = D3D11_USAGE_DEFAULT;

		device->CreateTexture2D(&targetTex, nullptr, &frameBuffer.targetTexturePtr);
		
		D3D11_RENDER_TARGET_VIEW_DESC targetDesc;
		targetDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		targetDesc.Texture2D.MipSlice = 0u;
		targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		device->CreateRenderTargetView(frameBuffer.targetTexturePtr, &targetDesc, &frameBuffer.targetPtr);

		// depth stencil state
		CD3D11_TEXTURE2D_DESC dsTex;
		dsTex.Width = width;
		dsTex.Height = height;
		dsTex.ArraySize = 1u;
		dsTex.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsTex.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsTex.MipLevels = 1u;
		dsTex.MiscFlags = 0u;
		dsTex.SampleDesc.Count = 1u;
		dsTex.SampleDesc.Quality = 0u;
		dsTex.CPUAccessFlags = 0u;
		dsTex.Usage = D3D11_USAGE_DEFAULT;

		device->CreateTexture2D(&dsTex, nullptr, &frameBuffer.dsTexturePtr);

		D3D11_DEPTH_STENCIL_DESC stateDesc;
	
		stateDesc.DepthEnable = TRUE;
		stateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		stateDesc.DepthFunc = D3D11_COMPARISON_LESS;
		stateDesc.StencilEnable = FALSE;

		device->CreateDepthStencilState(&stateDesc, &frameBuffer.dsStatePtr);

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0u;
		viewDesc.Flags = 0u;

		if(frameBuffer.dsTexturePtr)
			device->CreateDepthStencilView(frameBuffer.dsTexturePtr, &viewDesc, &frameBuffer.dsViewPtr);

		frameBuffer.state = 1;

		return ID;
	}

	void SetDepthState(bool enable, jsh::FrameBuffer fb)
	{
		if (fb == jsh::INVALID_FRAME_BUFFER) fb = g_MainFrameBuffer;
		FrameBuffer& frameBuffer = g_FrameBuffers[fb];

		D3D11_DEPTH_STENCIL_DESC stateDesc = {};
		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
		if (enable) {
			if (frameBuffer.state == 1 || frameBuffer.state == 3) return;

			frameBuffer.dsStatePtr->Release();
			frameBuffer.dsViewPtr->Release();

			stateDesc.DepthEnable = TRUE;
			stateDesc.DepthFunc = D3D11_COMPARISON_LESS;
			stateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			if (frameBuffer.state == 2) {
				jshLogW("Stencil state isn't implemented yet");
			}
			else stateDesc.StencilEnable = FALSE;

			frameBuffer.state = frameBuffer.state == 2 ? 3 : 1;

		}
		else {
			if (frameBuffer.state == 0 || frameBuffer.state == 2) return;

			frameBuffer.dsStatePtr->Release();
			frameBuffer.dsViewPtr->Release();

			stateDesc.DepthEnable = FALSE;
			if (frameBuffer.state == 3) {
				jshLogW("Stencil state isn't implemented yet");
			}
			else stateDesc.StencilEnable = FALSE;

			frameBuffer.state = frameBuffer.state == 3 ? 2 : 0;
		}

		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0u;
		viewDesc.Flags = 0u;

		device->CreateDepthStencilState(&stateDesc, &frameBuffer.dsStatePtr);
		device->CreateDepthStencilView(frameBuffer.dsTexturePtr, &viewDesc, &frameBuffer.dsViewPtr);
	}
	void SetStencilState(bool enable, jsh::FrameBuffer fb)
	{
		jshLogW("Stencil state isn't implemented yet");
	}

	jsh::Texture CreateTexture(void* data, uint32 pitch, uint32 width, uint32 height, JSH_FORMAT format)
	{
		jsh::Texture ID = g_Textures.push();
		Texture& texture = g_Textures[ID];

		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize = 1u;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0u;
		texDesc.Format = ParseFormat(format);
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1u;
		texDesc.MiscFlags = 0u;
		texDesc.SampleDesc.Count = 1u;
		texDesc.SampleDesc.Quality = 0u;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA subDesc;
		subDesc.pSysMem = data;
		subDesc.SysMemPitch = pitch;

		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		viewDesc.Format = ParseFormat(format);
		viewDesc.Texture2D.MipLevels = 1u;
		viewDesc.Texture2D.MostDetailedMip = 0u;
		
		device->CreateTexture2D(&texDesc, &subDesc, &texture.texturePtr);
		if (texture.texturePtr) device->CreateShaderResourceView(texture.texturePtr, &viewDesc, &texture.viewPtr);
		
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		device->CreateSamplerState(&desc, &texture.samplerStatePtr);

		return ID;
	}

	void SetSamplerState(jsh::Texture ID, JSH_FILTER filter, JSH_TEXTURE_ADDRESS_MODE addressMode)
	{
		Texture& texture = g_Textures[ID];	

		texture.samplerStatePtr->Release();
		
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		device->CreateSamplerState(&desc, &texture.samplerStatePtr);
	}

	/////////////////////////BINDING////////////////////////////////////////

	void BindVertexBuffer(jsh::Buffer ID, uint32 slot)
	{
		assert(ID != jsh::INVALID_BUFFER);

		Buffer& buffer = g_Buffers[ID];

		const UINT strides = buffer.structureByteStride;
		const UINT offset = 0U;
		context->IASetVertexBuffers(slot, 1, &buffer.ptr, &strides, &offset);

		return;
	}
	void BindIndexBuffer(jsh::Buffer ID)
	{
		assert(ID != jsh::INVALID_BUFFER);

		Buffer& buffer = g_Buffers[ID];
		context->IASetIndexBuffer(buffer.ptr, DXGI_FORMAT_R32_UINT, 0);

		return;
	}
	void BindConstantBuffer(jsh::Buffer ID, uint32 slot, JSH_SHADER_TYPE shaderType)
	{
		assert(ID != jsh::INVALID_BUFFER);

		Buffer& buffer = g_Buffers[ID];

		switch (shaderType) {
		case JSH_SHADER_TYPE_VERTEX:
			context->VSSetConstantBuffers(slot, 1, &buffer.ptr);
			break;
		case JSH_SHADER_TYPE_PIXEL:
			context->PSSetConstantBuffers(slot, 1, &buffer.ptr);
			break;
		case JSH_SHADER_TYPE_GEOMETRY:
			context->GSSetConstantBuffers(slot, 1, &buffer.ptr);
			break;
		case JSH_SHADER_TYPE_NULL:
		default:
			jshLogE("Invalid shader type");
			break;
		}

		return;
	}
	void UpdateConstantBuffer(jsh::Buffer ID, void* data)
	{
		Buffer& buffer = g_Buffers[ID];
		context->UpdateSubresource(buffer.ptr, 0, nullptr, data, 0, 0);
	}

	void BindInputLayout(jsh::InputLayout ID)
	{
		InputLayout& inputLayout = g_InputLayouts[ID];
		context->IASetInputLayout(inputLayout.ptr);
	}

	void BindVertexShader(jsh::VertexShader ID)
	{
		VertexShader& vs = g_VertexShaders[ID];
		context->VSSetShader(vs.ptr, nullptr, 0);
	}
	void BindPixelShader(jsh::PixelShader ID)
	{
		PixelShader& ps = g_PixelShaders[ID];
		context->PSSetShader(ps.ptr, nullptr, 0);
	}

	void BindFrameBuffer(jsh::FrameBuffer fb)
	{
		if (fb == jsh::INVALID_FRAME_BUFFER) fb = g_MainFrameBuffer;
		
		FrameBuffer& frameBuffer = g_FrameBuffers[fb];
		context->OMSetDepthStencilState(frameBuffer.dsStatePtr, 1u);
		context->OMSetRenderTargets(1u, &frameBuffer.targetPtr, frameBuffer.dsViewPtr);
	}

	void BindTexture(jsh::Texture ID, uint32 slot, JSH_SHADER_TYPE shaderType)
	{
		// unbind
		if (ID == jsh::INVALID_TEXTURE) {

			ID3D11SamplerState* s = {nullptr};
			ID3D11ShaderResourceView* t = {nullptr};

			switch (shaderType)
			{
			case JSH_SHADER_TYPE_VERTEX:
				context->VSSetShaderResources(slot, 1u, &t);
				break;
			case JSH_SHADER_TYPE_PIXEL:
				context->PSSetShaderResources(slot, 1u, &t);
				break;
			case JSH_SHADER_TYPE_GEOMETRY:
				context->GSSetShaderResources(slot, 1u, &t);
				break;
			case JSH_SHADER_TYPE_NULL:
			default:
				jshLogE("Invalid shader type");
				break;
			}
			return;
		}

		Texture& texture = g_Textures[ID];

		switch (shaderType)
		{
		case JSH_SHADER_TYPE_VERTEX:
			context->VSSetShaderResources(slot, 1u, &texture.viewPtr);
			context->VSSetSamplers(slot, 1u, &texture.samplerStatePtr);
			break;
		case JSH_SHADER_TYPE_PIXEL:
			context->PSSetShaderResources(slot, 1u, &texture.viewPtr);
			context->PSSetSamplers(slot, 1u, &texture.samplerStatePtr);
			break;
		case JSH_SHADER_TYPE_GEOMETRY:
			context->GSSetShaderResources(slot, 1u, &texture.viewPtr);
			context->GSSetSamplers(slot, 1u, &texture.samplerStatePtr);
			break;
		case JSH_SHADER_TYPE_NULL:
		default:
			jshLogE("Invalid shader type");
			break;
		}
	}

	///////////////////////////RENDER CALLS/////////////////////////
	void DrawIndexed(uint32 indicesCount)
	{
		context->DrawIndexed(indicesCount, 0, 0);
	}

}
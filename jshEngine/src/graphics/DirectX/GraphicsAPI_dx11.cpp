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
		D3D11_BUFFER_DESC desc;
		D3D11_SUBRESOURCE_DATA data;
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

	////////////////////ALLOCATION//////////////////////////

	jsh::memory_pool<Buffer> g_Buffers;
	jsh::memory_pool<InputLayout> g_InputLayouts;

	jsh::memory_pool<VertexShader> g_VertexShaders;
	jsh::memory_pool<PixelShader> g_PixelShaders;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11RenderTargetView* target = nullptr;

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
		ID3D11Resource* backBufferResource = nullptr;
		swapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&backBufferResource));

		if (backBufferResource) device->CreateRenderTargetView(backBufferResource, nullptr, &target);
		else return false;

		context->OMSetRenderTargets(1, &target, nullptr);

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
		if (device)		device->Release();
		if (context)	context->Release();
		if (swapChain)	swapChain->Release();
		if (target)		target->Release();

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

		return true;
	}

	void Prepare(float* clearScreenColor)
	{
		jshImGui(ImGui::Render());
		jshImGui(ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()));

		swapChain->Present(1u, 0u);
		context->ClearRenderTargetView(target, clearScreenColor);

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

		buffer.desc.BindFlags = ParseBindFlag(bufferType);
		buffer.desc.ByteWidth = size;
		buffer.desc.CPUAccessFlags = (usage == JSH_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;
		buffer.desc.MiscFlags = 0u;
		buffer.desc.StructureByteStride = stride;
		buffer.desc.Usage = ParseUsage(usage);

		buffer.data.pSysMem = data;
		buffer.data.SysMemPitch = 0u;
		buffer.data.SysMemSlicePitch = 0u;

		device->CreateBuffer(&buffer.desc, &buffer.data, &buffer.ptr);

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

	/////////////////////////BINDING////////////////////////////////////////

	void BindVertexBuffer(jsh::Buffer ID, uint32 slot)
	{
		assert(ID != jsh::INVALID_BUFFER);

		Buffer& buffer = g_Buffers[ID];

		const UINT strides = buffer.desc.StructureByteStride;
		const UINT offset = 0U;
		context->IASetVertexBuffers(slot, 1, &buffer.ptr, &strides, &offset);

		return;
	}
	void BindIndexBuffer(jsh::Buffer ID, uint32 slot)
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

	///////////////////////////RENDER CALLS/////////////////////////
	void DrawIndexed(uint32 indicesCount)
	{
		context->DrawIndexed(indicesCount, 0, 0);
	}

}
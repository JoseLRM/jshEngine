#include "Graphics.h"

#include "DirectX11Lib.h"
#include "Debug.h"
#include "WinLib.h"
#include "Window.h"

ID3D11Device* device = nullptr;
ID3D11DeviceContext* context = nullptr;
IDXGISwapChain* swapChain = nullptr;
ID3D11RenderTargetView* target = nullptr;

using namespace jsh;

namespace jshGraphics {

	float clearScreenColor[] = {0.f, 0.f, 0.f, 1.f};

	bool Initialize()
	{
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
		SetTopology(JSH_TOPOLOGY_TRIANGLES);

		// viewport
		D3D11_VIEWPORT viewport;
		viewport.Width = (float) jshWindow::GetWidth();
		viewport.Height = (float) jshWindow::GetHeight();
		viewport.MaxDepth = 1.f;
		viewport.MinDepth = 0.f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		context->RSSetViewports(1, &viewport);

		return true;
	}

	bool Close()
	{
		if(device)		device->Release();
		if(context)		context->Release();
		if(swapChain)	swapChain->Release();
		if(target)		target->Release();
		return true;
	}

	void Prepare()
	{
		swapChain->Present(1u, 0u);
		context->ClearRenderTargetView(target, clearScreenColor);
		/*
		

		ID3D11Buffer* vertexBuffer = nullptr;
		D3D11_BUFFER_DESC vbDesc = {};
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.ByteWidth = sizeof(Vertex) * std::size(vertexData);
		vbDesc.CPUAccessFlags = 0;
		vbDesc.MiscFlags = 0u;
		vbDesc.StructureByteStride = sizeof(Vertex);
		vbDesc.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA svbDesc = {};
		svbDesc.pSysMem = &vertexData;

		device->CreateBuffer(&vbDesc, &svbDesc, &vertexBuffer);


		ID3D11Buffer* indexBuffer = nullptr;
		D3D11_BUFFER_DESC ibDesc = {};
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibDesc.ByteWidth = sizeof(uint32) * std::size(indexData);
		ibDesc.CPUAccessFlags = 0;
		ibDesc.MiscFlags = 0u;
		ibDesc.StructureByteStride = sizeof(uint32);
		ibDesc.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA sibDesc = {};
		sibDesc.pSysMem = &indexData;

		device->CreateBuffer(&ibDesc, &sibDesc, &indexBuffer);

		const UINT vstrides = sizeof(Vertex);
		const UINT voffsets = 0;
		context->IASetVertexBuffers(0, 1, &vertexBuffer, &vstrides, &voffsets);

		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		*/
		
	}

	void SetTopology(JSH_TOPOLOGY topology) {

		D3D11_PRIMITIVE_TOPOLOGY d3dTopology;

		switch (topology) {
		case JSH_TOPOLOGY_LINES:
			d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			break;
		case JSH_TOPOLOGY_LINE_STRIP:
			d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
			break;
		case JSH_TOPOLOGY_POINTS:
			d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
			break;
		case JSH_TOPOLOGY_TRIANGLES:
			d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		case JSH_TOPOLOGY_TRIANGLE_STRIP:
			d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			break;
		default:
			jshLogE("Invalid topology");
			return;
		}

		context->IASetPrimitiveTopology(d3dTopology);
	}

	void SetClearScreenColor(float c)
	{
		clearScreenColor[0] = c;
		clearScreenColor[1] = c;
		clearScreenColor[2] = c;
	}

	void SetClearScreenColor(float r, float g, float b)
	{
		clearScreenColor[0] = r;
		clearScreenColor[1] = g;
		clearScreenColor[2] = b;
	}

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

	// ---------------Graphics device------------

	void CreateBuffer(Buffer& buffer)
	{
		ID3D11Buffer* ptr = nullptr;

		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = buffer.desc.BindFlags;
		desc.ByteWidth = buffer.desc.ByteWidth;
		desc.CPUAccessFlags = buffer.desc.CPUAccessFlags;
		desc.MiscFlags = buffer.desc.MiscFlags;
		desc.StructureByteStride = buffer.desc.StructureByteStride;
		desc.Usage = ParseUsage(buffer.desc.Usage);

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = buffer.data.pSysMem;
		data.SysMemPitch = buffer.data.SysMemPitch;
		data.SysMemSlicePitch = buffer.data.SysMemSlicePitch;

		device->CreateBuffer(&desc, &data, &ptr);

		buffer.ptr = reinterpret_cast<void*>(ptr);
	}

	void CreateVertexBuffer(jsh::Buffer& buffer, void* pData, uint32 bufferSize, uint32 stride)
	{
		buffer.desc.BindFlags = JSH_BIND_VERTEX_BUFFER;
		buffer.desc.ByteWidth = bufferSize;
		buffer.desc.CPUAccessFlags = 0;
		buffer.desc.MiscFlags = 0u;
		buffer.desc.StructureByteStride = stride;
		buffer.desc.Usage = JSH_USAGE_DEFAULT;

		buffer.data.pSysMem = pData;
		buffer.data.SysMemPitch = 0;
		buffer.data.SysMemSlicePitch = 0;

		CreateBuffer(buffer);
	}

	void CreateIndexBuffer(jsh::Buffer& buffer, void* pData, uint32 indexSize, uint32 indexCount)
	{
		buffer.desc.BindFlags = JSH_BIND_INDEX_BUFFER;
		buffer.desc.ByteWidth = indexSize * indexCount;
		buffer.desc.CPUAccessFlags = 0;
		buffer.desc.MiscFlags = 0u;
		buffer.desc.StructureByteStride = indexSize;
		buffer.desc.Usage = JSH_USAGE_DEFAULT;

		buffer.data.pSysMem = pData;
		buffer.data.SysMemPitch = 0;
		buffer.data.SysMemSlicePitch = 0;

		CreateBuffer(buffer);
	}

	void* GetDevice()
	{
		return (void*)device;
	}
	void* GetContext()
	{
		return (void*)context;
	}
}

namespace jsh {

	void Resource::Clear()
	{
		ID3D11Resource* pRes = reinterpret_cast<ID3D11Resource*>(ptr);
		pRes->Release();
	}

}
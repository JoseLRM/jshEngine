#include "Graphics.h"

#include "DirectX/GraphicsAPI_dx11.h"
#include "Debug.h"

JSH_GRAPHICS_API g_GraphicsAPI = JSH_GRAPHCS_API_NULL;

using namespace jsh;

namespace jshGraphics {

	float g_ClearScreenColor[] = {0.f, 0.f, 0.f, 1.f};

	bool Initialize()
	{
		g_GraphicsAPI = JSH_GRAPHCS_API_DIRECTX11;

		return jshGraphics_dx11::Initialize();
	}

#ifdef JSH_IMGUI
	bool InitializeImGui()
	{
		return jshGraphics_dx11::InitializeImGui();
	}
#endif

	bool Close()
	{
		return jshGraphics_dx11::Close();
	}

	void Prepare()
	{
		jshGraphics_dx11::Prepare(g_ClearScreenColor);
	}

	void SetTopology(JSH_TOPOLOGY topology) {
		jshGraphics_dx11::SetTopology(topology);
	}

	void SetClearScreenColor(float c)
	{
		g_ClearScreenColor[0] = c;
		g_ClearScreenColor[1] = c;
		g_ClearScreenColor[2] = c;
	}

	void SetClearScreenColor(float r, float g, float b)
	{
		g_ClearScreenColor[0] = r;
		g_ClearScreenColor[1] = g;
		g_ClearScreenColor[2] = b;
	}

	///////////////GRAPHICS API//////////////////////////////////////

	////////////////////CREATE/////////////////////////
	jsh::Buffer CreateBuffer(void* data, uint32 size, uint32 stride, JSH_USAGE usage, JSH_BUFFER_TYPE bufferType)
	{
		switch (g_GraphicsAPI) {

		case JSH_GRAPHCS_API_DIRECTX11:
			return jshGraphics_dx11::CreateBuffer(data, size, stride, usage, bufferType);

		case JSH_GRAPHCS_API_NULL:
		default:
			//TODO: throw exception
			jshDebug::ShowOkWindow(L"Undefined Graphics API", 3u);
			return INVALID_BUFFER;
		}
	}

	jsh::InputLayout CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* descriptors, uint32 cant, jsh::VertexShader vs)
	{
		return jshGraphics_dx11::CreateInputLayout(descriptors, cant, vs);
	}

	jsh::VertexShader CreateVertexShader(const wchar* path)
	{
		return jshGraphics_dx11::CreateVertexShader(path);
	}
	jsh::VertexShader CreatePixelShader(const wchar* path)
	{
		return jshGraphics_dx11::CreatePixelShader(path);
	}

	//////////////////////BIND////////////////

	void BindVertexBuffer(jsh::Buffer buffer, uint32 slot)
	{
		switch (g_GraphicsAPI)
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
	void BindIndexBuffer(jsh::Buffer buffer, uint32 slot)
	{
		switch (g_GraphicsAPI)
		{
		case JSH_GRAPHCS_API_DIRECTX11:
			jshGraphics_dx11::BindIndexBuffer(buffer, slot);
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
		switch (g_GraphicsAPI)
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

	void BindInputLayout(jsh::InputLayout inputLayout)
	{
		jshGraphics_dx11::BindInputLayout(inputLayout);
	}

	void BindVertexShader(jsh::VertexShader vs)
	{
		jshGraphics_dx11::BindVertexShader(vs);
	}
	void BindPixelShader(jsh::PixelShader ps)
	{
		jshGraphics_dx11::BindPixelShader(ps);
	}

	//////////////////DRAW CALLS//////////////////
	void DrawIndexed(uint32 indicesCount)
	{
		jshGraphics_dx11::DrawIndexed(indicesCount);
	}

}
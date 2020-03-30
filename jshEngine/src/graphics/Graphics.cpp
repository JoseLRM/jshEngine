#include "Graphics.h"

#include "DirectX/GraphicsAPI_dx11.h"
#include "..//utils/dataStructures/memory_pool.h"
#include "Debug.h"
#include <map>
#include "3D/Renderer3D.h"

JSH_GRAPHICS_API g_GraphicsAPI = JSH_GRAPHCS_API_NULL;

using namespace jsh;

namespace jshGraphics {

	float g_ClearScreenColor[] = {0.f, 0.f, 0.f, 1.f};

	bool Initialize()
	{
		g_GraphicsAPI = JSH_GRAPHCS_API_DIRECTX11;

		bool result = jshGraphics_dx11::Initialize();

		// default shaders
		Shader solidShader;
		solidShader.vs = jshGraphics::CreateVertexShader(L"SolidVertex.cso");
		solidShader.ps = jshGraphics::CreatePixelShader(L"SolidPixel.cso");
		jshGraphics::CreateShader("SolidShader", solidShader);

		jshRenderer3D::Initialize();

		return result;
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

	JSH_GRAPHICS_API GetAPI()
	{
		return g_GraphicsAPI;
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

	void UpdateConstantBuffer(jsh::Buffer buffer, void* data)
	{
		jshGraphics_dx11::UpdateConstantBuffer(buffer, data);
	}

	//////////////////DRAW CALLS//////////////////
	void DrawIndexed(uint32 indicesCount)
	{
		jshGraphics_dx11::DrawIndexed(indicesCount);
	}

}
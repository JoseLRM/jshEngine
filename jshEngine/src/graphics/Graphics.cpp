#include "Graphics.h"

#include "DirectX/GraphicsAPI_dx11.h"
#include "..//utils/dataStructures/memory_pool.h"
#include "Debug.h"
#include <map>
#include "Renderer.h"

JSH_GRAPHICS_API g_GraphicsAPI = JSH_GRAPHCS_API_NULL;

using namespace jsh;

namespace jshGraphics {

	bool Initialize()
	{
		g_GraphicsAPI = JSH_GRAPHCS_API_DIRECTX11;

		bool result = jshGraphics_dx11::Initialize();

		// default shaders
		std::shared_ptr<Shader> solidShader = std::make_shared<Shader>();
		jshGraphics::CreateVertexShader(L"SolidVertex.cso", &solidShader->vs);
		jshGraphics::CreatePixelShader(L"SolidPixel.cso", &solidShader->ps);
		jshGraphics::Save("SolidShader", solidShader);

		std::shared_ptr<Shader> simpleTexShader = std::make_shared<Shader>();
		jshGraphics::CreateVertexShader(L"SimpleTexVertex.cso", &simpleTexShader->vs);
		jshGraphics::CreatePixelShader(L"SimpleTexPixel.cso", &simpleTexShader->ps);
		jshGraphics::Save("SimpleTexShader", simpleTexShader);

		FrameBuffer::Initialize();

		jshRenderer::Initialize();

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

	void Begin()
	{
		jshGraphics_dx11::Begin();
	}
	void End()
	{
		jshGraphics_dx11::End();
	}
	void Present()
	{
		jshGraphics_dx11::Present();
	}

#ifdef JSH_IMGUI
	void BeginImGui()
	{
		jshGraphics_dx11::BeginImGui();
	}
	void EndImGui(const RenderTargetView& rtv)
	{
		jshGraphics_dx11::EndImGui(rtv);
	}
#endif

	CommandList BeginCommandList()
	{
		return jshGraphics_dx11::BeginCommandList();
	}

	JSH_GRAPHICS_API GetAPI()
	{
		return g_GraphicsAPI;
	}

	void SetTopology(JSH_TOPOLOGY topology, jsh::CommandList cmd) {
		jshGraphics_dx11::SetTopology(topology, cmd);
	}

	///////////////GRAPHICS API//////////////////////////////////////

	void UpdateConstantBuffer(jsh::Buffer buffer, void* data, CommandList cmd)
	{
		jshGraphics_dx11::UpdateConstantBuffer(buffer, data, cmd);
	}

	//////////////////DRAW CALLS//////////////////
	void DrawIndexed(uint32 indicesCount, CommandList cmd)
	{
		jshGraphics_dx11::DrawIndexed(indicesCount, cmd);
	}

}
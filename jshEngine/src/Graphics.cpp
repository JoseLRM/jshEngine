#include "Graphics.h"

#include "GraphicsAPI_dx11.h"
#include "memory_pool.h"
#include "PostProcess.h"
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
		{
			Shader* solidShader = jshGraphics::CreateShader("Solid");
			jshGraphics::CreateVertexShader(L"SolidVertex.cso", &solidShader->vs);
			jshGraphics::CreatePixelShader(L"SolidPixel.cso", &solidShader->ps);

			const JSH_INPUT_ELEMENT_DESC desc[] = {
				{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u},
				{"Normal", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 3 * sizeof(float), 0u}
			};

			jshGraphics::CreateInputLayout(desc, 2u, solidShader->vs, &solidShader->inputLayout);
		}

		{
			Shader* simpleTexShader = jshGraphics::CreateShader("SimpleTex");
			jshGraphics::CreateVertexShader(L"SimpleTexVertex.cso", &simpleTexShader->vs);
			jshGraphics::CreatePixelShader(L"SimpleTexPixel.cso", &simpleTexShader->ps);
			
			const JSH_INPUT_ELEMENT_DESC desc[] = {
					{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u},
					{"Normal", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 3 * sizeof(float), 0u},
					{"TexCoord", 0, JSH_FORMAT_R32G32_FLOAT, 0, true, 6 * sizeof(float), 0u}
			};

			jshGraphics::CreateInputLayout(desc, 3u, simpleTexShader->vs, &simpleTexShader->inputLayout);
		}

		{
			Shader* simpleColShader = jshGraphics::CreateShader("SimpleCol");
			jshGraphics::CreateVertexShader(L"SimpleColVertex.cso", &simpleColShader->vs);
			jshGraphics::CreatePixelShader(L"SimpleColPixel.cso", &simpleColShader->ps);

			const JSH_INPUT_ELEMENT_DESC desc[] = {
				{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u},
				{"Normal", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 3 * sizeof(float), 0u},
				{"Color", 0, JSH_FORMAT_R8G8B8A8_UNORM, 0, true, 6 * sizeof(float), 0u}
			};

			jshGraphics::CreateInputLayout(desc, 3u, simpleColShader->vs, &simpleColShader->inputLayout);

		}

		{
			Shader* normalShader = jshGraphics::CreateShader("Normal");
			jshGraphics::CreateVertexShader(L"NormalVertex.cso", &normalShader->vs);
			jshGraphics::CreatePixelShader(L"NormalPixel.cso", &normalShader->ps);
			
			const JSH_INPUT_ELEMENT_DESC desc[] = {
				{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u},
				{"Normal", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 3 * sizeof(float), 0u},
				{"TexCoord", 0, JSH_FORMAT_R32G32_FLOAT, 0, true, 6 * sizeof(float), 0u},
				{"Tangent", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 8 * sizeof(float), 0u},
				{"Bitangent", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 11 * sizeof(float), 0u}
			};

			jshGraphics::CreateInputLayout(desc, 5u, normalShader->vs, &normalShader->inputLayout);
		}

		{
			Shader* outlineShader = jshGraphics::CreateShader("OutlineMask");
			jshGraphics::CreateVertexShader(L"OutlineMaskVertex.cso", &outlineShader->vs);
			jshGraphics::CreatePixelShader(L"OutlineMaskPixel.cso", &outlineShader->ps);

			const JSH_INPUT_ELEMENT_DESC desc[] = {
				{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u}
			};

			jshGraphics::CreateInputLayout(desc, 1u, outlineShader->vs, &outlineShader->inputLayout);
		}

		jshRenderer::Initialize();
		jshRenderer::InitializePostProcess();

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
		jshGraphics::ClearObjects();
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
	void Present(uint32 interval)
	{
		jshGraphics_dx11::Present(interval);
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

	void UpdateConstantBuffer(jsh::Resource buffer, void* data, CommandList cmd)
	{
		jshGraphics_dx11::UpdateConstantBuffer(buffer, data, cmd);
	}

	//////////////////DRAW CALLS//////////////////
	void DrawIndexed(uint32 indicesCount, CommandList cmd)
	{
		jshGraphics_dx11::DrawIndexed(indicesCount, cmd);
	}
	void Draw(uint32 vertexCount, jsh::CommandList cmd)
	{
		jshGraphics_dx11::Draw(vertexCount, cmd);
	}

}
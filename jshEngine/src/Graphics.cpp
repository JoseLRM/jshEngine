#include "Graphics.h"

#include "GraphicsAPI_dx11.h"
#include "PostProcess.h"
#include "Debug.h"
#include <map>
#include "Renderer.h"
#include "Window.h"
#include "EventSystem.h"

using namespace jsh;

namespace jshGraphics {

	JSH_GRAPHICS_API g_GraphicsAPI = JSH_GRAPHCS_API_NULL;
	uvec2 g_Resolution;
	bool g_Fullscreen;

	bool Initialize()
	{
		g_GraphicsAPI = JSH_GRAPHCS_API_DIRECTX11;

		g_Resolution = { (uint32)jshWindow::GetWidth(), (uint32)jshWindow::GetHeight() };

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

		{
			Shader* spriteShader = jshGraphics::CreateShader("Sprite");
			jshGraphics::CreateVertexShader(L"SpriteVertex.cso", &spriteShader->vs);
			jshGraphics::CreatePixelShader(L"SpritePixel.cso", &spriteShader->ps);

			const JSH_INPUT_ELEMENT_DESC desc[] = {
				{"Position", 0, JSH_FORMAT_R32G32_FLOAT, 0, true, 0u, 0u},
				{"TM", 0, JSH_FORMAT_R32G32B32A32_FLOAT, 1, false, 0u, 1u},
				{"TM", 1, JSH_FORMAT_R32G32B32A32_FLOAT, 1, false, 4 * sizeof(float), 1u},
				{"TM", 2, JSH_FORMAT_R32G32B32A32_FLOAT, 1, false, 8 * sizeof(float), 1u},
				{"TM", 3, JSH_FORMAT_R32G32B32A32_FLOAT, 1, false, 12 * sizeof(float), 1u},
				{"Color", 0, JSH_FORMAT_R32G32B32A32_FLOAT, 1, false, 16 * sizeof(float), 1u},
				{"TexCoord", 0, JSH_FORMAT_R32G32B32A32_FLOAT, 1, false, 20 * sizeof(float), 1u},
				{"TextureID", 0, JSH_FORMAT_R16_UINT, 1, false, 24 * sizeof(float), 1u},
			};

			jshGraphics::CreateInputLayout(desc, 8u, spriteShader->vs, &spriteShader->inputLayout);
		}

		jshGraphics::primitives::Initialize();
		jshGraphics::InitializePostProcess();

		return result;
	}

	bool Close()
	{
		jshGraphics::ClearObjects();
		jshGraphics::SetFullscreen(false);
		return jshGraphics_dx11::Close();
	}

	void Begin()
	{
		jshImGui(jshGraphics::BeginImGui());
		jshGraphics_dx11::Begin();
	}
	void End()
	{
		jshGraphics_dx11::End();
		jshImGui(jshGraphics::EndImGui(jshGraphics::GetRenderTargetViewFromBackBuffer()));
	}
	void Present(uint32 interval)
	{
		jshGraphics_dx11::Present(interval);
	}

#ifdef JSH_IMGUI
	bool InitializeImGui()
	{
		return jshGraphics_dx11::InitializeImGui();
	}
	void BeginImGui()
	{
		jshGraphics_dx11::BeginImGui();
	}
	void EndImGui(const RenderTargetView& rtv)
	{
		jshGraphics_dx11::EndImGui(rtv);
	}
#endif

	JSH_GRAPHICS_API GetAPI()
	{
		return g_GraphicsAPI;
	}

	//////////////////////////RESOLUTION///////////////////////
	void SetResolution(uint32 width, uint32 height)
	{
		if (width == 0 || height == 0) {
			jshLogW("Invalid Resolution: %u, %u", width, height);
			return;
		}
		if (width == g_Resolution.x && height == g_Resolution.y) return;

		jshGraphics_dx11::SetResolution(width, height);

		g_Resolution = uvec2(width, height);

		jsh::ResolutionEvent e(width, height);
		jshEvent::Dispatch(e);
	}
	uvec2 GetResolution()
	{
		return g_Resolution;
	}
	void SetFullscreen(bool fullscreen)
	{
		if (g_Fullscreen == fullscreen) return;

		jshGraphics_dx11::SetFullscreen(fullscreen);
		g_Fullscreen = fullscreen;

		jsh::FullscreenEvent e(fullscreen);
		jshEvent::Dispatch(e);
	}
	bool InFullscreen()
	{
		return g_Fullscreen;
	}
}
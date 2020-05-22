#include "common.h"

#include "Graphics.h"

#include "GraphicsAPI_dx11.h"
#include "PostProcess.h"
#include <map>
#include "Renderer.h"
#include "Window.h"
#include "EventSystem.h"

using namespace jsh;

namespace jshGraphics {

	JSH_GRAPHICS_API g_GraphicsAPI = JSH_GRAPHCS_API_NULL;
	bool g_Fullscreen;
	uvec2 g_Resolution;
	bool g_VSync = false;

	namespace _internal {
		bool Initialize()
		{
			g_GraphicsAPI = JSH_GRAPHCS_API_DIRECTX11;

			bool result = jshGraphics_dx11::Initialize();

			{
				bool fullscreen = true;
#ifdef JSH_DEBUG
				fullscreen = false;
#endif 
				g_Resolution = jshGraphics::GetOutputMode().resolution;
				uint32 width = jshGraphics::GetResolutionWidth();
				uint32 height = jshGraphics::GetResolutionHeight();
				jshWindow::SetBounds(GetMonitorResolution().x / 2 - width / 2, GetMonitorResolution().y / 2 - height / 2, width, height);
				SetFullscreen(fullscreen);
			}

			jshGraphics::primitives::Initialize();
			jshGraphics::objects::Initialize();
			jshGraphics::InitializePostProcess();

			return result;
		}

		bool Close()
		{
			jshGraphics::ClearObjects();
			jshGraphics::SetFullscreen(false);

			jshGraphics::_internal::ReleasePrimitives();

			return jshGraphics_dx11::Close();
		}
		void Present(jsh::RenderTargetView& rtv)
		{
			uint32 interval = g_VSync ? 1u : 0u;
			jshGraphics_dx11::Present(rtv, interval);
		}

#ifdef JSH_IMGUI
		bool InitializeImGui()
		{
			bool result = jshGraphics_dx11::InitializeImGui();

			// Load style file
			jsh::Archive file;
			if (file.LoadFile("ImGuiStyle.jsh"))
			{
				file >> ImGui::GetStyle();
			}

			return result;
		}
		bool CloseImGui()
		{
			return jshGraphics_dx11::CloseImGui();
		}
		bool ShowImGuiWindow()
		{
			bool result = true;
			static bool showAdapters = false;

			if (ImGui::Begin("Graphics")) {

				if (ImGui::Button(showAdapters ? "Hide Adapters" : "Show Adapters")) showAdapters = !showAdapters;

				if (ImGui::Button(g_VSync ? "Disable VSync" : "Enable VSync")) g_VSync = !g_VSync;

				// resolution
				int maxResolution = jshGraphics::GetOutputMode().resolution.x;
				int newWidth = g_Resolution.x;
				ImGui::SliderInt("Resolution", &newWidth, 10, maxResolution);
				SetResolution(newWidth);

				if (ImGui::Button("Close")) result = false;
			}
			ImGui::End();

			if (showAdapters) {
				if (ImGui::Begin("Adapters")) {

					auto& adapters = GetAdapters();

					for (uint32 i = 0; i < adapters.size(); ++i) {
						const GraphicsAdapter& adapter = adapters[i];

						if (ImGui::TreeNode("Adapter")) {

							auto& modes = adapter.GetModes();
							for (uint32 j = 0; j < modes.size(); ++j) {
								const OutputMode& mode = modes[j];
								ImGui::Separator();
								ImGui::Text("Resolution: %u / %u", mode.resolution.x, mode.resolution.y);
								if (ImGui::Button((std::string("Set Mode") + std::to_string(j)).c_str())) jshGraphics::SetOutputMode(mode);
							}

							ImGui::TreePop();
						}
					}
				}
				ImGui::End();
			}

			return result;
		}

	}
	
#endif

	JSH_GRAPHICS_API GetAPI()
	{
		return g_GraphicsAPI;
	}

	//////////////////////////FULLSCREEN///////////////////////
	void SetFullscreen(bool fullscreen)
	{
		if (g_Fullscreen == fullscreen) return;

		jshGraphics_dx11::SetFullscreen(fullscreen, jshGraphics::GetOutputMode());
		g_Fullscreen = fullscreen;

		jsh::FullscreenEvent e(fullscreen);
		jshEvent::Dispatch(e);
	}
	bool InFullscreen()
	{
		return g_Fullscreen;
	}

	/////////////////////////////RESOLUTION/////////////////////////////
	uint32 GetResolutionWidth()
	{
		return g_Resolution.x;
	}
	uint32 GetResolutionHeight()
	{
		return g_Resolution.y;
	}
	uvec2 GetResolution()
	{
		return g_Resolution;
	}
	float GetResolutionAspect()
	{
		return (float)g_Resolution.x / (float)g_Resolution.y;
	}
	void SetResolution(float width, float height)
	{
		g_Resolution.x = width;
		g_Resolution.y = height;

		jsh::ResolutionEvent e(g_Resolution.x, g_Resolution.y);
		jshEvent::Dispatch(e);
	}
	void SetResolution(float width)
	{
		if (width == g_Resolution.x) return;
		// Mantein the aspect of the window
		float w = jshWindow::GetWidth();
		float h = jshWindow::GetHeight();
		float windowAspect = w / h;

		SetResolution(width, width / windowAspect);
	}
	void _internal::SetResolutionAspect(float aspect)
	{
		if (GetResolutionAspect() == aspect) return;
		SetResolution(g_Resolution.x, g_Resolution.x / aspect);
	}

	///////////////////////////////SHADERS///////////////////////////

	void objects::Initialize()
	{
		s_SolidShader.Create();
		s_NormalShader.Create();
	}

	SolidShader objects::s_SolidShader;
	NormalShader objects::s_NormalShader;

}
#include "Graphics.h"

#include "GraphicsAPI_dx11.h"
#include "PostProcess.h"
#include "Debug.h"
#include <map>
#include "Renderer.h"
#include "Window.h"
#include "EventSystem.h"
#include "Engine.h"

using namespace jsh;

namespace jshGraphics {

	JSH_GRAPHICS_API g_GraphicsAPI = JSH_GRAPHCS_API_NULL;
	bool g_Fullscreen;

	bool Initialize()
	{
		g_GraphicsAPI = JSH_GRAPHCS_API_DIRECTX11;

		bool result = jshGraphics_dx11::Initialize();

		{
			
			bool fullscreen = true;
#ifdef JSH_DEBUG
			//fullscreen = false;
#endif 
			uint32 width = jshGraphics::GetResolutionWidth();
			uint32 height = jshGraphics::GetResolutionHeight();
			jshWindow::SetBounds(GetMonitorResolution().x / 2 - width / 2, GetMonitorResolution().y / 2 - height / 2, width, height);
			SetFullscreen(fullscreen);
		}

		/*

		// default shaders
		{
			Shader* solidShader = jshGraphics::CreateShader("Solid");

			jshGraphics::CreateVertexShader(L"SolidVertex.cso", &solidShader->GetVertexShader());
			jshGraphics::CreatePixelShader(L"SolidPixel.cso", &solidShader->GetPixelShader());

			solidShader->AddVertexProperty("Position", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
			solidShader->AddVertexProperty("Normal", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
		}

		{
			Shader* simpleTexShader = jshGraphics::CreateShader("Solid");

			jshGraphics::CreateVertexShader(L"SolidVertex.cso", &simpleTexShader->GetVertexShader());
			jshGraphics::CreatePixelShader(L"SolidPixel.cso", &simpleTexShader->GetPixelShader());

			simpleTexShader->AddVertexProperty("Position", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
			simpleTexShader->AddVertexProperty("Normal", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
			simpleTexShader->AddVertexProperty("TexCoord", JSH_FORMAT_R32G32_FLOAT, 0u);
		}

		{
			Shader* simpleColShader = jshGraphics::CreateShader("SimpleColor");

			jshGraphics::CreateVertexShader(L"SimpleColVertex.cso", &simpleColShader->GetVertexShader());
			jshGraphics::CreatePixelShader(L"SimpleColPixel.cso", &simpleColShader->GetPixelShader());

			simpleColShader->AddVertexProperty("Position", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
			simpleColShader->AddVertexProperty("Normal", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
			simpleColShader->AddVertexProperty("Color", JSH_FORMAT_R8G8B8A8_UNORM, 0u);
		}

		{
			Shader* normalShader = jshGraphics::CreateShader("Normal");

			jshGraphics::CreateVertexShader(L"NormalVertex.cso", &normalShader->GetVertexShader());
			jshGraphics::CreatePixelShader(L"NormalPixel.cso", &normalShader->GetPixelShader());

			normalShader->AddVertexProperty("Position", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
			normalShader->AddVertexProperty("Normal", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
			normalShader->AddVertexProperty("TexCoord", JSH_FORMAT_R32G32_FLOAT, 0u);
			normalShader->AddVertexProperty("Tangent", JSH_FORMAT_R32G32B32_FLOAT, 0u);
			normalShader->AddVertexProperty("Bitangent", JSH_FORMAT_R32G32B32_FLOAT, 0u);
		}

		{
			Shader* outlineShader = jshGraphics::CreateShader("OutlineMask");

			jshGraphics::CreateVertexShader(L"OutlineMaskVertex.cso", &outlineShader->GetVertexShader());
			jshGraphics::CreatePixelShader(L"OutlineMaskPixel.cso", &outlineShader->GetPixelShader());

			outlineShader->AddVertexProperty("Position", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
		}

		{
			Shader* spriteShader = jshGraphics::CreateShader("Sprite");
			jshGraphics::CreateVertexShader(L"SpriteVertex.cso", &spriteShader->GetVertexShader());
			jshGraphics::CreatePixelShader(L"SpritePixel.cso", &spriteShader->GetPixelShader());

			spriteShader->AddVertexProperty("Position", JSH_FORMAT_R32G32_FLOAT, 0u);
			spriteShader->AddVertexProperty("TM", JSH_FORMAT_R32G32_FLOAT, 0u);
			spriteShader->AddVertexProperty("TM", JSH_FORMAT_R32G32_FLOAT, 1u);
			spriteShader->AddVertexProperty("TM", JSH_FORMAT_R32G32_FLOAT, 2u);
			spriteShader->AddVertexProperty("TM", JSH_FORMAT_R32G32_FLOAT, 3u);
			spriteShader->AddVertexProperty("Color", JSH_FORMAT_R8G8B8A8_UNORM, 0u);
			spriteShader->AddVertexProperty("TexCoord", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
			spriteShader->AddVertexProperty("TextureID", JSH_FORMAT_R16_UINT, 0u);
		}
		*/

		jshGraphics::primitives::Initialize();
		jshGraphics::objects::Initialize();
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
	bool ShowImGuiWindow()
	{
		bool result = true;
		static bool showAdapters = false;

		if (ImGui::Begin("Graphics")) {

			if (ImGui::Button(showAdapters ? "Hide Adapters" : "Show Adapters")) showAdapters = !showAdapters;

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

	bool GenerateInputLayout(jsh::Shader* shader, jsh::RawData* rawData, jsh::InputLayout* il)
	{
		auto& sProps = shader->GetVertexProperties();
		auto& instanceProps = shader->GetInstanceProperties();
		auto& rLayout = rawData->GetVertexLayout();

		// align instance layout
		{
			uint32 count = 0u;
			for (uint32 i = 0; i < instanceProps.size(); ++i) {
				count += jshGraphics::GetFormatStride(instanceProps[i].format);
			}
			if (count > JSH_GFX_MAX_INSTANCE_SIZE) {
				throw jshException(L"Graphics Exception", L"You are exceeded the instance data limits, change 'JSH_GFX_MAX_INSTANCE_SIZE' in config.h");
			}
		}

		uint32 descSize = uint32(sProps.size() + instanceProps.size());
		JSH_INPUT_ELEMENT_DESC* desc = new JSH_INPUT_ELEMENT_DESC[descSize];

		// create vertex input elements
		for (uint32 i = 0; i < sProps.size(); ++i) {
			bool find = false;
			uint32 j;
			VertexProperty& prop = sProps[i];

			for (j = 0; j < rLayout.size(); ++j) {
				if (prop == rLayout[j].prop) {
					find = true;
					break;
				}
			}
			if (!find) return false;

			desc[i] = { prop.name, prop.index, prop.format, 0u, true, rLayout[j].offset, 0u };
		}

		// create instance input elements
		uint32 offset = 0u;
		{
			uint32 instanceCount = 0u;
			for (uint32 i = uint32(sProps.size()); i < descSize; ++i) {
				VertexProperty& prop = instanceProps[instanceCount++];
				desc[i] = { prop.name, prop.index, prop.format, 1u, false, offset, 1u };
				offset += jshGraphics::GetFormatStride(prop.format);
			}
		}

		jshGraphics::CreateInputLayout(desc, descSize, shader->GetVertexShader(), il);

		delete[] desc;
		return true;
	}

	///////////////////////////////SHADERS///////////////////////////

	void objects::Initialize()
	{
		// Sprite RawData
		{
			float data[] = {
				-0.5f,  0.5f,
				 0.5f,  0.5f,
				-0.5f, -0.5f,
				 0.5f, -0.5f
			};

			s_SpriteRawData.AddVertexLayout("Position", JSH_FORMAT_R32G32_FLOAT, 0u, data);
			s_SpriteRawData.SetTopology(JSH_TOPOLOGY_TRIANGLE_STRIP);

			s_SpriteRawData.SetVertexCount(4);

			s_SpriteRawData.Create();
		}

		// Sprite shader
		{
			s_SpriteShader = Shader(88);

			jshGraphics::CreateVertexShader(L"SpriteVertex.cso", &s_SpriteShader.GetVertexShader());
			jshGraphics::CreatePixelShader(L"SpritePixel.cso", &s_SpriteShader.GetPixelShader());

			s_SpriteShader.AddVertexProperty("Position", JSH_FORMAT_R32G32_FLOAT, 0u);

			s_SpriteShader.AddInstanceProperty("TM", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
			s_SpriteShader.AddInstanceProperty("TM", JSH_FORMAT_R32G32B32A32_FLOAT, 1u);
			s_SpriteShader.AddInstanceProperty("TM", JSH_FORMAT_R32G32B32A32_FLOAT, 2u);
			s_SpriteShader.AddInstanceProperty("TM", JSH_FORMAT_R32G32B32A32_FLOAT, 3u);
			s_SpriteShader.AddInstanceProperty("TexCoord", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
			s_SpriteShader.AddInstanceProperty("Color", JSH_FORMAT_R8G8B8A8_UNORM, 0u);
			s_SpriteShader.AddInstanceProperty("Texture", JSH_FORMAT_R32_UINT, 0u);
		}

		// Solid shader
		{
			jshGraphics::CreateVertexShader(L"SolidVertex.cso", &s_SolidShader.GetVertexShader());
			jshGraphics::CreatePixelShader(L"SolidPixel.cso", &s_SolidShader.GetPixelShader());

			s_SolidShader.AddVertexProperty("Position", JSH_FORMAT_R32G32B32_FLOAT, 0u);
			s_SolidShader.AddVertexProperty("Normal", JSH_FORMAT_R32G32B32_FLOAT, 0u);

			s_SolidShader.AddInstanceProperty("TM", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
			s_SolidShader.AddInstanceProperty("TM", JSH_FORMAT_R32G32B32A32_FLOAT, 1u);
			s_SolidShader.AddInstanceProperty("TM", JSH_FORMAT_R32G32B32A32_FLOAT, 2u);
			s_SolidShader.AddInstanceProperty("TM", JSH_FORMAT_R32G32B32A32_FLOAT, 3u);
		}

		// Normal shader
		{
			jshGraphics::CreateVertexShader(L"NormalVertex.cso", &s_NormalShader.GetVertexShader());
			jshGraphics::CreatePixelShader(L"NormalPixel.cso", &s_NormalShader.GetPixelShader());

			s_NormalShader.AddVertexProperty("Position", JSH_FORMAT_R32G32B32_FLOAT, 0u);
			s_NormalShader.AddVertexProperty("Normal", JSH_FORMAT_R32G32B32_FLOAT, 0u);
			s_NormalShader.AddVertexProperty("TexCoord", JSH_FORMAT_R32G32_FLOAT, 0u);
			s_NormalShader.AddVertexProperty("Tangent", JSH_FORMAT_R32G32B32_FLOAT, 0u);
			s_NormalShader.AddVertexProperty("Bitangent", JSH_FORMAT_R32G32B32_FLOAT, 0u);

			s_NormalShader.AddInstanceProperty("TM", JSH_FORMAT_R32G32B32A32_FLOAT, 0u);
			s_NormalShader.AddInstanceProperty("TM", JSH_FORMAT_R32G32B32A32_FLOAT, 1u);
			s_NormalShader.AddInstanceProperty("TM", JSH_FORMAT_R32G32B32A32_FLOAT, 2u);
			s_NormalShader.AddInstanceProperty("TM", JSH_FORMAT_R32G32B32A32_FLOAT, 3u);
		}
	}

	SolidShader objects::s_SolidShader;
	NormalShader objects::s_NormalShader;
	Shader objects::s_SpriteShader;
	RawData objects::s_SpriteRawData;

}
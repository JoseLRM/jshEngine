#include "common.h"

#include "Renderer.h"
#include "Graphics.h"
#include "RenderGraph.h"

#include "PostProcessTechnique.h"
#include "ClearPass.h"

#include "MeshRenderer.h"
#include "SpriteRenderer.h"

using namespace jsh;

namespace jshRenderer {

	RenderTargetView	g_OffscreenRTV;
	RenderTargetView	g_AuxOffscreenRTV;
	TextureRes			g_DepthStencilView;
	Viewport			g_Viewport;

	RenderGraph*		g_RenderGraph = nullptr;
	CameraComponent*	g_Camera = nullptr;

	jshEvent::Listener<ResolutionEvent> g_ResolutionListener;

	MeshRenderer*	g_MeshRenderer;
	SpriteRenderer* g_SpriteRenderer;

	// Environment
	vec3 g_AmbientLight = { 0.1f, 0.1f, 0.1f };

	namespace _internal {

		bool Initialize()
		{
			if(!jshGraphics::_internal::Initialize()) return false;

			// Create Offscreen RTV
			{
				JSH_RENDER_TARGET_VIEW_DESC desc;
				desc.Format = JSH_FORMAT_R8G8B8A8_UNORM;
				desc.Texture2D.MipSlice = 0u;
				desc.ViewDimension = JSH_RTV_DIMENSION_TEXTURE2D;

				JSH_TEXTURE2D_DESC resDesc;
				resDesc.ArraySize = 1u;
				resDesc.BindFlags = JSH_BIND_RENDER_TARGET | JSH_BIND_SHADER_RESOURCE;
				resDesc.CPUAccessFlags = 0u;
				resDesc.Format = JSH_FORMAT_R8G8B8A8_UNORM;
				resDesc.Width = jshGraphics::GetResolutionWidth();
				resDesc.Height = jshGraphics::GetResolutionHeight();
				resDesc.MipLevels = 0u;
				resDesc.MiscFlags = 0u;
				resDesc.SampleDesc.Count = 1u;
				resDesc.SampleDesc.Quality = 0u;
				resDesc.Usage = JSH_USAGE_DEFAULT;

				jshGraphics::CreateRenderTargetView(&desc, &resDesc, &g_OffscreenRTV);
				jshGraphics::CreateRenderTargetView(&desc, &resDesc, &g_AuxOffscreenRTV);
			}
			// DEPTH STENCIL VIEW & VIEWPORT
			{
				JSH_TEXTURE2D_DESC dsResDesc;
				jshZeroMemory(&dsResDesc, sizeof(JSH_TEXTURE2D_DESC));
				dsResDesc.ArraySize = 1u;
				dsResDesc.BindFlags = JSH_BIND_DEPTH_STENCIL;
				dsResDesc.CPUAccessFlags = 0u;
				dsResDesc.Format = JSH_FORMAT_D24_UNORM_S8_UINT;
				dsResDesc.Width = jshGraphics::GetResolutionWidth();
				dsResDesc.Height = jshGraphics::GetResolutionHeight();
				dsResDesc.MipLevels = 1u;
				dsResDesc.MiscFlags = 0u;
				dsResDesc.SampleDesc.Count = 1u;
				dsResDesc.SampleDesc.Quality = 0u;
				dsResDesc.Usage = JSH_USAGE_DEFAULT;

				jshGraphics::CreateTextureRes(&dsResDesc, nullptr, &g_DepthStencilView);
				jshGraphics::CreateViewport(0.f, 0.f, jshGraphics::GetResolutionWidth(), jshGraphics::GetResolutionHeight(), &g_Viewport);
			}

			// Register resolution event
			g_ResolutionListener.Register(JSH_EVENT_LAYER_SYSTEM, [](ResolutionEvent& e) {

				jshGraphics::ResizeRenderTargetView(g_OffscreenRTV, e.width, e.height);
				jshGraphics::ResizeRenderTargetView(g_AuxOffscreenRTV, e.width, e.height);
				JSH_TEXTURE2D_DESC desc = jshGraphics::GetTextureDesc(g_DepthStencilView);
				desc.Width = e.width;
				desc.Height = e.height;
				jshGraphics::CreateTextureRes(&desc, nullptr, &g_DepthStencilView);
				jshGraphics::CreateViewport(0.f, 0.f, e.width, e.height, &g_Viewport);

				return true;
			});

			// Create Default Render Graph
			{
				g_RenderGraph = new RenderGraph();

				ClearPass* clearPass = new ClearPass();

				clearPass->AddRenderTargetView(g_OffscreenRTV, 0.f, 0.f, 0.f, 0.f);
				clearPass->AddDepthStencilView(g_DepthStencilView);

				g_MeshRenderer = new MeshRenderer(g_OffscreenRTV, g_DepthStencilView, g_Viewport);
				g_SpriteRenderer = new SpriteRenderer(g_OffscreenRTV, g_Viewport);

				g_RenderGraph->AddTechnique(clearPass);
				g_RenderGraph->AddTechnique(g_MeshRenderer);
				g_RenderGraph->AddTechnique(g_SpriteRenderer);
				g_RenderGraph->AddTechnique(new PostProcessTechnique(g_OffscreenRTV, g_AuxOffscreenRTV, g_Viewport));

				g_RenderGraph->Create();
			}

			return true;
		}
		bool Close()
		{
			if (g_RenderGraph) delete g_RenderGraph;

			if (!jshGraphics::_internal::Close()) return false;

			return true;
		}
		void Begin()
		{
			jshGraphics::BeginCommandList();
			jshGraphics::ClearRenderTargetView(jshGraphics::GetRenderTargetViewFromBackBuffer(), 0.f, 0.f, 0.f, 0.f, 0);
			
			g_SpriteRenderer->Begin(g_Camera->GetCamera());
		}
		void Render()
		{
			g_MeshRenderer->DrawScene(g_Camera->GetCamera());
			g_SpriteRenderer->DrawScene();

			g_SpriteRenderer->End();

			if (g_RenderGraph) {
				g_RenderGraph->Render(&g_Camera->GetCamera());
			}
		}
		void End()
		{
			jshGraphics::_internal::Present(g_OffscreenRTV);
		}

#ifdef JSH_IMGUI
		bool ShowImGuiWindow()
		{
			bool result = true;

			if (ImGui::Begin("Renderer")) {
				ImGui::ColorPicker3("Ambient Light", &g_AmbientLight.x);
				if (ImGui::Button("Close")) result = false;
			}
			ImGui::End();
			return result;
		}
#endif

	}

	//////////////GETTERS AND SETTERS//////////////////////
	void SetRenderGraph(jsh::RenderGraph* rg)
	{
		if (g_RenderGraph) delete g_RenderGraph;
		g_RenderGraph = rg;
	}
	jsh::RenderGraph* GetRenderGraph()
	{
		return g_RenderGraph;
	}

	void SetCamera(jsh::CameraComponent* camera)
	{
		g_Camera = camera;
	}
	CameraComponent* GetCamera()
	{
		return g_Camera;
	}

	namespace environment {
		void SetAmbientLight(float r, float g, float b)
		{
			g_AmbientLight = { r,g,b };
		}
		jsh::vec3 GetAmbientLight()
		{
			return g_AmbientLight;
		}
	}

	SpriteRenderer& GetSpriteRenderer()
	{
		return *g_SpriteRenderer;
	}
	MeshRenderer& GetMeshRenderer()
	{
		return *g_MeshRenderer;
	}
}
#include "Renderer.h"

#include "..//ecs/Scene.h"
#include "..//components/Components.h"
#include "Graphics.h"
#include "..//utils/AssetLoader.h"
#include "..//Engine.h"

#include "renderPasses/LambertianRenderPass.h"
#include "renderPasses/PostProcessingRenderPass.h"
#include "RenderGraph.h"

using namespace jsh;

namespace jshRenderer {

	namespace primitives {
		jsh::RenderTargetView	g_MainRenderTargetView;
		jsh::DepthStencilState	g_DefaultDepthStencilState;
		jsh::Texture			g_DefaultDepthStencilView;

		jsh::RenderTargetView	g_OffscreenRenderTargetView;

		jsh::SamplerState		g_DefaultSamplerState;
		jsh::Viewport			g_DefaultViewport;

		// getters
		jsh::RenderTargetView& GetMainRenderTargetView() { return g_MainRenderTargetView; }
		jsh::DepthStencilState& GetDefaultDepthStencilState() { return g_DefaultDepthStencilState; }
		jsh::Texture& GetDefaultDepthStencilView() { return g_DefaultDepthStencilView; }

		jsh::RenderTargetView& GetOffscreenRenderTargetView() { return g_OffscreenRenderTargetView; }

		jsh::SamplerState& GetDefaultSamplerState() { return g_DefaultSamplerState; }
		jsh::Viewport& GetDefaultViewport() { return g_DefaultViewport; }
	}

	using namespace primitives;

	class DefRenderGraph : public RenderGraph {

	public:
		void Initialize() override 
		{
			LambertianRenderPass* lambertianPass = new LambertianRenderPass();
			lambertianPass->Create();

			PostProcessingRenderPass* ppPass = new PostProcessingRenderPass();
			ppPass->Create();

			AddPass(lambertianPass);
			AddPass(ppPass);
		}
	};

	CameraComponent* g_MainCamera = nullptr;
	DefRenderGraph g_RenderGraph;

	bool Initialize()
	{		
		// MAIN FRAME BUFFER
		{
			JSH_RENDER_TARGET_VIEW_DESC rtvDesc;
			jshZeroMemory(&rtvDesc, sizeof(JSH_RENDER_TARGET_VIEW_DESC));
			rtvDesc.Format = JSH_FORMAT_B8G8R8A8_UNORM;
			rtvDesc.ViewDimension = JSH_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0u;

			JSH_DEPTH_STENCIL_DESC dsDesc;
			jshZeroMemory(&dsDesc, sizeof(JSH_DEPTH_STENCIL_DESC));
			dsDesc.DepthEnable = true;
			dsDesc.DepthFunc = JSH_COMPARISON_LESS;
			dsDesc.DepthWriteMask = JSH_DEPTH_WRITE_MASK_ALL;
			dsDesc.StencilEnable = FALSE;

			JSH_TEXTURE2D_DESC dsResDesc;
			jshZeroMemory(&dsResDesc, sizeof(JSH_TEXTURE2D_DESC));
			dsResDesc.ArraySize = 1u;
			dsResDesc.BindFlags = JSH_BIND_DEPTH_STENCIL;
			dsResDesc.CPUAccessFlags = 0u;
			dsResDesc.Format = JSH_FORMAT_D24_UNORM_S8_UINT;
			dsResDesc.Width = 1080;
			dsResDesc.Height = 720;
			dsResDesc.MipLevels = 1u;
			dsResDesc.MiscFlags = 0u;
			dsResDesc.SampleDesc.Count = 1u;
			dsResDesc.SampleDesc.Quality = 0u;
			dsResDesc.Usage = JSH_USAGE_DEFAULT;

			jshGraphics::CreateRenderTargetViewFromBackBuffer(&rtvDesc, &g_MainRenderTargetView);
			jshGraphics::CreateDepthStencilState(&dsDesc, &g_DefaultDepthStencilState);
			jshGraphics::CreateTexture(&dsResDesc, nullptr, &g_DefaultDepthStencilView);
		}

		// OFFSREEN RTV
		{
			JSH_RENDER_TARGET_VIEW_DESC rtvDesc;
			rtvDesc.Format = JSH_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.Texture2D.MipSlice = 0u;
			rtvDesc.ViewDimension = JSH_RTV_DIMENSION_TEXTURE2D;

			JSH_TEXTURE2D_DESC res;
			res.ArraySize = 1u;
			res.BindFlags = JSH_BIND_RENDER_TARGET | JSH_BIND_SHADER_RESOURCE;
			res.CPUAccessFlags = 0u;
			res.Format = JSH_FORMAT_R8G8B8A8_UNORM;
			res.Width = 1080;
			res.Height = 720;
			res.MipLevels = 1u;
			res.MiscFlags = 0u;
			res.SampleDesc.Count = 1u;
			res.SampleDesc.Quality = 0u;
			res.Usage = JSH_USAGE_DEFAULT;

			jshGraphics::CreateRenderTargetView(&rtvDesc, &res, &g_OffscreenRenderTargetView);
		}

		// DEFAULT SAMPLER
		JSH_SAMPLER_DESC samplerDesc;
		jshZeroMemory(&samplerDesc, sizeof(JSH_SAMPLER_DESC));
		samplerDesc.AddressU = JSH_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = JSH_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = JSH_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = JSH_FILTER_MIN_MAG_MIP_LINEAR;
		jshGraphics::CreateSamplerState(&samplerDesc, &g_DefaultSamplerState);

		// DEFAULT VIEWPORT
		jshGraphics::CreateViewport(0, 0, 1080, 720, &g_DefaultViewport);

		g_RenderGraph.Initialize();

		return true;
	}

	void BeginFrame()
	{
		jshImGui(jshGraphics::BeginImGui());
		jshGraphics::Begin();
		jshGraphics::BeginCommandList();
	}

	void EndFrame()
	{
		g_RenderGraph.Render();

		jshGraphics::End();
		jshImGui(jshGraphics::EndImGui(g_MainRenderTargetView));
		jshGraphics::Present();	
	}

	bool Close()
	{
		return true;
	}

	void SetCamera(jsh::CameraComponent* camera)
	{
		camera->UpdateMatrices();
		g_MainCamera = camera;
	}

	jsh::CameraComponent* GetMainCamera()
	{
		return g_MainCamera;
	}

}
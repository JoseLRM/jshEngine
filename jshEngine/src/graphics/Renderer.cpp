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
		std::shared_ptr<FrameBuffer> mainFB = std::make_shared<FrameBuffer>();
		jshGraphics::Save("MainFrameBuffer", mainFB);
		mainFB->Create(true);

		std::shared_ptr<SamplerState> defSampler = std::make_shared<SamplerState>();
		jshGraphics::Save("DefaultSamplerState", defSampler);
		JSH_SAMPLER_DESC samplerDesc;
		jshZeroMemory(&samplerDesc, sizeof(JSH_SAMPLER_DESC));
		samplerDesc.AddressU = JSH_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = JSH_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = JSH_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = JSH_FILTER_MIN_MAG_MIP_LINEAR;
		jshGraphics::CreateSamplerState(&samplerDesc, defSampler.get());

		std::shared_ptr<Viewport> defViewport = std::make_shared<Viewport>();
		jshGraphics::Save("DefaultViewport", defViewport);
		jshGraphics::CreateViewport(0, 0, 1080, 720, defViewport.get());

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
		FrameBuffer* mainFB = (FrameBuffer*)jshGraphics::Get("MainFrameBuffer");
		jshImGui(jshGraphics::EndImGui(mainFB->GetRTV()));
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
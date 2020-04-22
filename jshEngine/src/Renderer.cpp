#include "Renderer.h"

#include "Scene.h"
#include "Components.h"
#include "Graphics.h"
#include "AssetLoader.h"
#include "Engine.h"

#include "LambertianRenderPass.h"
#include "OutlineRenderPass.h"
#include "RenderGraph.h"

using namespace jsh;

namespace jshRenderer {

	bool g_VSYNC = false;

	namespace primitives {

		//////////////////////DEFAULT PRIMITIVES////////////////////////
		jsh::RenderTargetView	g_MainRenderTargetView;
		jsh::DepthStencilState	g_DefaultDepthStencilState;
		jsh::DepthStencilState	g_DisabledDepthStencilState;
		jsh::Resource			g_DefaultDepthStencilView;

		jsh::RenderTargetView	g_OffscreenRenderTargetView;

		jsh::SamplerState		g_DefaultSamplerState;
		jsh::Viewport			g_DefaultViewport;

		jsh::BlendState			g_DefaultBlendState;
		jsh::BlendState			g_TransparentBlendState;

		////////////////////GLOBAL CBUFFERS///////////////////////////
		Resource g_CameraBuffer;
		Resource g_LightBuffer;

		struct {
			XMMATRIX vm;
			XMMATRIX pm;

			jsh::vec4 position;
			jsh::vec4 aux0;
			jsh::vec4 aux1;
			jsh::vec4 aux2;
		} g_CameraBufferData;

		struct Light {
			vec4 lightPos;
			vec4 data;
			vec4 color;
		};
		struct alignas(16) {
			Light lights[JSH_GFX_MAX_LIGHTS];
		} g_LightBufferData;

		//////////////////////////GETTERS////////////////////////
		jsh::RenderTargetView& GetMainRenderTargetView() { return g_MainRenderTargetView; }
		jsh::DepthStencilState& GetDefaultDepthStencilState() { return g_DefaultDepthStencilState; }
		jsh::DepthStencilState& GetDisabledDepthStencilState() { return g_DisabledDepthStencilState; }
		jsh::Resource& GetDefaultDepthStencilView() { return g_DefaultDepthStencilView; }

		jsh::RenderTargetView& GetOffscreenRenderTargetView() { return g_OffscreenRenderTargetView; }

		jsh::SamplerState& GetDefaultSamplerState() { return g_DefaultSamplerState; }
		jsh::Viewport& GetDefaultViewport() { return g_DefaultViewport; }

		jsh::Resource& GetCameraBuffer() { return g_CameraBuffer; }
		jsh::Resource& GetLightBuffer() { return g_LightBuffer; }

		jsh::BlendState& GetTransparentBlendState() { return g_TransparentBlendState; }
		jsh::BlendState& GetDefaultBlendState() { return g_DefaultBlendState; }
	}

	using namespace primitives;

	Entity g_MainCamera = INVALID_ENTITY;
	RenderGraph g_RenderGraph;

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
			dsDesc.StencilEnable = false;

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
			jshGraphics::CreateResource(&dsResDesc, nullptr, &g_DefaultDepthStencilView);
		}

		// DISABLED DSS
		{
			JSH_DEPTH_STENCIL_DESC desc;
			jshZeroMemory(&desc, sizeof(JSH_DEPTH_STENCIL_DESC));
			desc.DepthEnable = false;
			desc.StencilEnable = false;
			desc.DepthWriteMask = JSH_DEPTH_WRITE_MASK_ZERO;
			jshGraphics::CreateDepthStencilState(&desc, &g_DisabledDepthStencilState);
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

		// DEFAULT SAMPLER & VIEWPORT
		{
			JSH_SAMPLER_DESC samplerDesc;
			jshZeroMemory(&samplerDesc, sizeof(JSH_SAMPLER_DESC));
			samplerDesc.AddressU = JSH_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = JSH_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = JSH_TEXTURE_ADDRESS_WRAP;
			samplerDesc.Filter = JSH_FILTER_MIN_MAG_MIP_LINEAR;
			jshGraphics::CreateSamplerState(&samplerDesc, &g_DefaultSamplerState);
			jshGraphics::CreateViewport(0, 0, 1080, 720, &g_DefaultViewport);
		}

		// DEFAULT BLEND STATES
		{
			JSH_BLEND_DESC desc;
			jshZeroMemory(&desc, sizeof(JSH_BLEND_DESC));
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = JSH_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = JSH_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = JSH_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = JSH_COLOR_WRITE_ENABLE_ALL;

			desc.RenderTarget[0].SrcBlendAlpha = JSH_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = JSH_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = JSH_BLEND_OP_ADD;
			
			jshGraphics::CreateBlendState(&desc, &g_TransparentBlendState);
		}
		{
			JSH_BLEND_DESC desc;
			jshZeroMemory(&desc, sizeof(JSH_BLEND_DESC));
			desc.RenderTarget[0].BlendEnable = false;
			desc.RenderTarget[0].RenderTargetWriteMask = JSH_COLOR_WRITE_ENABLE_ALL;

			jshGraphics::CreateBlendState(&desc, &g_DefaultBlendState);
		}

		// RENDER GRAPH
		{
			LambertianRenderPass* lambertianPass = new LambertianRenderPass();
			lambertianPass->Create();

			OutlineRenderPass* outlinePass = new OutlineRenderPass();
			outlinePass->Create();

			g_RenderGraph.AddPass(lambertianPass);
			g_RenderGraph.AddPass(outlinePass);
		}

		// GLOBAL BUFFERS
		{
			JSH_BUFFER_DESC cameraDesc;
			cameraDesc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			cameraDesc.ByteWidth = sizeof(g_CameraBufferData);
			cameraDesc.CPUAccessFlags = 0u;
			cameraDesc.MiscFlags = 0u;
			cameraDesc.StructureByteStride = 0u;
			cameraDesc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA cameraSData;
			cameraSData.pSysMem = &g_CameraBufferData;
			jshGraphics::CreateResource(&cameraDesc, &cameraSData, &g_CameraBuffer);

			JSH_BUFFER_DESC lightDesc;
			lightDesc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			lightDesc.ByteWidth = sizeof(g_LightBufferData);
			lightDesc.CPUAccessFlags = 0u;
			lightDesc.MiscFlags = 0u;
			lightDesc.StructureByteStride = 0u;
			lightDesc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA lightSData;
			lightSData.pSysMem = &g_LightBufferData;
			jshGraphics::CreateResource(&lightDesc, &lightSData, &g_LightBuffer);
		}

		return true;
	}

	void BeginFrame()
	{
		jshImGui(jshGraphics::BeginImGui());
		jshGraphics::Begin();	
	}

	void EndFrame()
	{
		bool canRender = true;

		CommandList cmd = jshGraphics::BeginCommandList();

		jshGraphics::ClearRenderTargetView(g_OffscreenRenderTargetView, 0.f, 0.f, 0.f, 1.f, cmd);
		jshGraphics::ClearRenderTargetView(g_MainRenderTargetView, 0.f, 0.f, 0.f, 1.f, cmd);
		jshGraphics::ClearDepthStencilView(g_DefaultDepthStencilView, cmd);

		// UPDATE GLOBAL BUFFERS
		{
			if (g_MainCamera != INVALID_ENTITY) {

				// camera
				auto camera = jshScene::GetComponent<jsh::CameraComponent>(g_MainCamera);

				Transform& cameraTransform = jshScene::GetTransform(g_MainCamera);

				camera->UpdateMatrices();
				g_CameraBufferData.vm = camera->GetViewMatrix();
				g_CameraBufferData.pm = camera->GetProjectionMatrix();
				const vec3 pos = cameraTransform.GetWorldPosition();
				g_CameraBufferData.position = vec4(pos.x, pos.y, pos.z, 1.f);
				jshGraphics::UpdateConstantBuffer(g_CameraBuffer, &g_CameraBufferData, cmd);

				// lights
				jshZeroMemory(&g_LightBufferData, sizeof(g_LightBufferData));
				auto& lightsList = jshScene::_internal::GetComponentsList()[LightComponent::ID];

				if (lightsList.size() / LightComponent::SIZE > JSH_GFX_MAX_LIGHTS) {
					jshLogE("Too many lights, there are %u lights availables", JSH_GFX_MAX_LIGHTS);
					canRender = false;
				}

				uint32 count = 0u;

				for (uint32 i = 0; i < lightsList.size(); i += LightComponent::SIZE) {
					LightComponent* lightComp = reinterpret_cast<LightComponent*>(&lightsList[i]);

					if (lightComp->lightType == 0) continue;

					Light& light = g_LightBufferData.lights[count++];
					light.color = lightComp->color;
					light.lightPos = *(vec4*)& jshScene::GetTransform(lightComp->entity).GetWorldPosition();
					light.data.x = lightComp->quadraticAttenuation;
					light.data.y = lightComp->constantAttenuation;
					light.data.z = lightComp->intensity;
					light.data.w = *reinterpret_cast<float*>(&lightComp->lightType);
				}
				jshGraphics::UpdateConstantBuffer(g_LightBuffer, &g_LightBufferData, cmd);
			}
			else {
				canRender = false;
				jshLogE("Where are the camera bro ;)");
			}
		}

		// render passes
		if(canRender) g_RenderGraph.Render();

		// end
		jshGraphics::End();
		jshImGui(jshGraphics::EndImGui(g_MainRenderTargetView));

		uint32 interval = 0u;
		if (g_VSYNC) interval = 1u;
		jshGraphics::Present(interval);	
	}

	bool Close()
	{
		return true;
	}

	///////////////////////SETTERS - GETTERS//////////////////////////
	void SetCamera(jsh::Entity camera)
	{
		g_MainCamera = camera;
	}

	jsh::CameraComponent* GetMainCamera()
	{
		return jshScene::GetComponent<CameraComponent>(g_MainCamera);
	}

#ifdef JSH_IMGUI
	bool ShowImGuiWindow()
	{
		bool result = true;
		if (ImGui::Begin("Renderer")) {

			ImGui::Checkbox("VSYNC", &g_VSYNC);

			if (ImGui::Button("Close")) result = false;
		}
		ImGui::End();
		return result;
	}
#endif

}
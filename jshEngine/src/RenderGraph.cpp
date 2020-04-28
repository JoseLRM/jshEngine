#include "RenderGraph.h"

#include "TaskSystem.h"
#include "Components.h"

namespace jsh {

	void RenderPass::AddDependence(RenderPass* renderPass) noexcept
	{
		if (renderPass == this) return;
		for (uint32 i = 0; i < m_Depencences.size(); ++i) {
			if (m_Depencences[i] == renderPass) return;
		}
		m_Depencences.push_back(renderPass);
	}

	RenderGraph::RenderGraph() {}

	void RenderGraph::Create()
	{
		// GLOBAL BUFFERS
		{
			JSH_BUFFER_DESC cameraDesc;
			cameraDesc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			cameraDesc.ByteWidth = sizeof(m_CameraBufferData);
			cameraDesc.CPUAccessFlags = 0u;
			cameraDesc.MiscFlags = 0u;
			cameraDesc.StructureByteStride = 0u;
			cameraDesc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA cameraSData;
			cameraSData.pSysMem = &m_CameraBufferData;
			jshGraphics::CreateBuffer(&cameraDesc, &cameraSData, &m_CameraBuffer);

			JSH_BUFFER_DESC lightDesc;
			lightDesc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			lightDesc.ByteWidth = sizeof(m_LightBufferData);
			lightDesc.CPUAccessFlags = 0u;
			lightDesc.MiscFlags = 0u;
			lightDesc.StructureByteStride = 0u;
			lightDesc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA lightSData;
			lightSData.pSysMem = &m_LightBufferData;
			jshGraphics::CreateBuffer(&lightDesc, &lightSData, &m_LightBuffer);
		}
	}

	void RenderGraph::Render(CameraComponent* camera)
	{
		// Clear
		CommandList cmd = jshGraphics::BeginCommandList();

		jshGraphics::ClearRenderTargetView(jshGraphics::primitives::GetOffscreenRenderTargetView(), 0.f, 0.f, 0.f, 1.f, cmd);
		jshGraphics::ClearRenderTargetView(jshGraphics::primitives::GetMainRenderTargetView(), 0.f, 0.f, 0.f, 1.f, cmd);
		jshGraphics::ClearDepthStencilView(jshGraphics::primitives::GetDefaultDepthStencilView(), cmd);

		//GLOBAL BUFFERS
		bool doFrame = true;
		m_pCurrentCamera = camera;

		if (camera != nullptr) {

			// camera
			Transform& cameraTransform = jshScene::GetTransform(camera->entity);

			camera->UpdateMatrices();
			m_CameraBufferData.vm = camera->GetViewMatrix();
			m_CameraBufferData.pm = camera->GetProjectionMatrix();
			const vec3 pos = cameraTransform.GetWorldPosition();
			m_CameraBufferData.position = vec4(pos.x, pos.y, pos.z, 1.f);
			jshGraphics::UpdateBuffer(m_CameraBuffer, &m_CameraBufferData, 0u, cmd);

			// lights
			jshZeroMemory(&m_LightBufferData, sizeof(m_LightBufferData));
			auto& lightsList = jshScene::_internal::GetComponentsList()[LightComponent::ID];

			if (lightsList.size() / LightComponent::SIZE > JSH_GFX_MAX_LIGHTS) {
				jshLogE("Too many lights, there are %u lights availables", JSH_GFX_MAX_LIGHTS);
			}

			uint32 count = 0u;

			for (uint32 i = 0; i < lightsList.size(); i += LightComponent::SIZE) {
				LightComponent* lightComp = reinterpret_cast<LightComponent*>(&lightsList[i]);

				if (lightComp->lightType == 0) continue;

				Light& light = m_LightBufferData.lights[count++];
				light.color = lightComp->color;
				light.lightPos = *(vec4*)& jshScene::GetTransform(lightComp->entity).GetWorldPosition();
				light.data.x = lightComp->quadraticAttenuation;
				light.data.y = lightComp->constantAttenuation;
				light.data.z = lightComp->intensity;
				light.data.w = *reinterpret_cast<float*>(&lightComp->lightType);
			}
			jshGraphics::UpdateBuffer(m_LightBuffer, &m_LightBufferData, 0u, cmd);
		}
		else {
			jshLogE("Where are the camera bro ;)");
			doFrame = false;
		}

		if (m_Modified) UpdateGraph();
		if (!doFrame) return;

		for (uint32 i = 0; i < m_RenderPasses.size(); ++i) {
			m_RenderPasses[i]->Load();
		}
		jshTask::Wait();

		for (uint32 i = 0; i < m_RenderPasses.size(); ++i) {
			m_RenderPasses[i]->Render(jshGraphics::BeginCommandList());
		}
	}

	void RenderGraph::UpdateGraph()
	{
		m_Modified = false;
		//TODO: Render graph
	}

}
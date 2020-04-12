#include "LambertianRenderPass.h"

#include "..//..//components/Components.h"
#include "..//..//Engine.h"
#include "..//Renderer.h"

namespace jsh {

	LambertianRenderPass::LambertianRenderPass()  : m_MatrixData()
	{
		m_MeshSystem.pList = &m_Instances;

		m_MeshSystem.AddRequestedComponents<MeshComponent>();

		m_MeshSystem.SetIndividualSystem();
		m_MeshSystem.SetName("Mesh Renderer");

		m_LightSystem.pLightData = &m_LightData;

		m_LightSystem.AddRequestedComponents<LightComponent>();

		m_LightSystem.SetIndividualSystem();
		m_LightSystem.SetName("Light Renderer");
	}

	void LambertianRenderPass::Create()
	{
		JSH_BUFFER_DESC cvDesc;
		cvDesc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
		cvDesc.ByteWidth = sizeof(MatrixData);
		cvDesc.CPUAccessFlags = 0u;
		cvDesc.MiscFlags = 0u;
		cvDesc.StructureByteStride = 0u;
		cvDesc.Usage = JSH_USAGE_DEFAULT;
		JSH_SUBRESOURCE_DATA cvSData;
		cvSData.pSysMem = &m_MatrixData;
		jshGraphics::CreateBuffer(&cvDesc, &cvSData, &m_MatrixBuffer);

		JSH_BUFFER_DESC cpDesc;
		cpDesc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
		cpDesc.ByteWidth = sizeof(LightData);
		cpDesc.CPUAccessFlags = 0u;
		cpDesc.MiscFlags = 0u;
		cpDesc.StructureByteStride = 0u;
		cpDesc.Usage = JSH_USAGE_DEFAULT;
		JSH_SUBRESOURCE_DATA cpSData;
		cpSData.pSysMem = &m_LightData;
		jshGraphics::CreateBuffer(&cpDesc, &cpSData, &m_LightBuffer);
	}
	void LambertianRenderPass::Render()
	{
		CameraComponent* camera = jshRenderer::GetMainCamera();
		if (!camera) return;

		jshScene::UpdateSystem(&m_MeshSystem, 0.f);
		jshScene::UpdateSystem(&m_LightSystem, 0.f);

		FrameBuffer* fb = (FrameBuffer*)jshGraphics::Get("MainFrameBuffer");
		fb->Bind(cmd);
		fb->Clear(cmd);

		fb->EnableDepthTest(cmd);
		fb->DisableStencilTest(cmd);

		jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLES, cmd);

		SamplerState* st = (SamplerState*)jshGraphics::Get("DefaultSamplerState");
		jshGraphics::BindSamplerState(*st, 0u, JSH_SHADER_TYPE_PIXEL, cmd);
		jshGraphics::BindSamplerState(*st, 1u, JSH_SHADER_TYPE_PIXEL, cmd);

		Viewport* vp = (Viewport*)jshGraphics::Get("DefaultViewport");
		jshGraphics::BindViewport(*vp, 0u, cmd);

		camera->UpdateMatrices();

		m_MatrixData.pm = camera->GetProjectionMatrix();
		m_MatrixData.vm = camera->GetViewMatrix();

		jshGraphics::BindConstantBuffer(m_MatrixBuffer, 0u, JSH_SHADER_TYPE_VERTEX, cmd);
		jshGraphics::BindConstantBuffer(m_LightBuffer, 0u, JSH_SHADER_TYPE_PIXEL, cmd);
		jshGraphics::UpdateConstantBuffer(m_LightBuffer, &m_LightData, cmd);

		for (uint32 i = 0; i < m_Instances.size(); ++i) {
			MeshInstance& instance = m_Instances[i];
			instance.mesh->Bind(cmd);
			m_MatrixData.tm = XMMatrixTranspose(instance.pTransform->GetWorldMatrix());

			jshGraphics::UpdateConstantBuffer(m_MatrixBuffer, &m_MatrixData, cmd);
			jshGraphics::DrawIndexed(instance.mesh->rawData->GetIndexCount(), cmd);
		}

		m_Instances.clear(false);
		m_LightSystem.Clear();
	}

	void MeshSystem::UpdateEntity(Entity e, BaseComponent** comp, float dt)
	{
		MeshComponent* meshComp = (MeshComponent*) comp[0];
		Transform* transform = &jshScene::GetTransform(e);

		meshComp->mesh->UpdatePrimitives();
		pList->push_back({ meshComp->mesh, transform }, 10);
	}

	void LightSystem::UpdateEntity(Entity e, BaseComponent** comp, float dt)
	{
		LightComponent* lightComp = (LightComponent*)comp[0];
		Transform* transform = &jshScene::GetTransform(e);

		lightComp->lightType = 1;

		Light& light = pLightData->lights[count];
		light.color = lightComp->color;
		light.lightPos = *(vec4*)&transform->GetWorldPosition();
		light.data.x = lightComp->quadraticAttenuation;
		light.data.y = lightComp->constantAttenuation;
		light.data.z = lightComp->intensity;
		light.data.w = *reinterpret_cast<float*>(&lightComp->lightType);

		count++;
	}

	void LightSystem::Clear()
	{
		jshZeroMemory(pLightData, sizeof(LightData));
		count = 0u;
	}
}
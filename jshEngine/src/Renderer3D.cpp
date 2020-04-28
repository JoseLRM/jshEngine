#include "Renderer3D.h"

namespace jsh {

	bool Renderer3D::Initialize()
	{
		m_Rendering3DPass.Create();
		m_OutlineRenderPass.Create();
		m_PostProcessRenderPass.Create();

		m_Rendering3DPass.SetMeshBatch(&m_MeshBatch);

		m_RenderGraph.AddPass(&m_Rendering3DPass);
		m_RenderGraph.AddPass(&m_OutlineRenderPass);
		m_RenderGraph.AddPass(&m_PostProcessRenderPass);

		m_RenderGraph.Create();

		return true;
	}

	void Renderer3D::Begin()
	{
		jshGraphics::Begin();
	}

	void Renderer3D::Render()
	{
		// Draw
		m_MeshBatch.Begin();

		auto meshList = jshScene::_internal::GetComponentsList()[MeshComponent::ID];
		m_MeshBatch.ReserveInstances(meshList.size() / MeshComponent::SIZE);

		for (uint32 i = 0; i < meshList.size(); i += MeshComponent::SIZE) {

			MeshComponent* meshComp = reinterpret_cast<MeshComponent*>(&meshList[i]);
			Transform& transform = jshScene::GetTransform(meshComp->entity);

			if (meshComp->mesh == nullptr) continue;
			assert(meshComp->mesh->GetShader() != nullptr && meshComp->mesh->GetRawData() != nullptr);

			// TODO: Frustum culling

			m_MeshBatch.Draw(&transform, meshComp->mesh);
		}

		m_RenderGraph.Render(jshScene::GetComponent<CameraComponent>(m_MainCamera));
	}

	void Renderer3D::End()
	{
		jshGraphics::End();
		jshGraphics::Present(0.f);
	}

	bool Renderer3D::Close()
	{
		return true;
	}

}
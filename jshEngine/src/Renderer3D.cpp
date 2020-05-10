#include "Renderer3D.h"

namespace jsh {

	bool Renderer3D::Initialize()
	{
		m_Rendering3DPass.Create();
		//m_OutlineRenderPass.Create();
		m_PostProcessRenderPass.Create();

		m_Rendering3DPass.SetRenderQueue(&m_RenderQueue);

		m_RenderGraph.AddPass(&m_Rendering3DPass);
		//m_RenderGraph.AddPass(&m_OutlineRenderPass);
		m_RenderGraph.AddPass(&m_PostProcessRenderPass);

		m_RenderGraph.Create();

		m_SpriteRenderQueue.SetShader(jshGraphics::objects::GetSpriteShader());
		m_Rendering3DPass.SetSpriteRenderQueue(&m_SpriteRenderQueue);

		return true;
	}

	void Renderer3D::Begin()
	{
		jshGraphics::Begin();
	}

	void Renderer3D::Render()
	{
		// Fill render queues
		{
			m_RenderQueue.Begin();

			auto& meshList = jshScene::_internal::GetComponentsList()[MeshComponent::ID];
			m_RenderQueue.Reserve(meshList.size() / MeshComponent::SIZE);

			for (uint32 i = 0; i < meshList.size(); i += MeshComponent::SIZE) {

				MeshComponent* meshComp = reinterpret_cast<MeshComponent*>(&meshList[i]);
				Transform& transform = jshScene::GetTransform(meshComp->entity);

				if (meshComp->mesh == nullptr) continue;

				// TODO: Frustum culling

				m_RenderQueue.Draw(meshComp->mesh, &transform);
			}
			m_RenderQueue.End();
		}

		{
			m_SpriteRenderQueue.Begin();

			auto& spriteList = jshScene::_internal::GetComponentsList()[SpriteComponent::ID];
			m_SpriteRenderQueue.Reserve(spriteList.size() / SpriteComponent::SIZE);

			for (uint32 i = 0; i < spriteList.size(); i += SpriteComponent::SIZE) {
				SpriteComponent* sprComp = reinterpret_cast<SpriteComponent*>(&spriteList[i]);
				Transform& trans = jshScene::GetTransform(sprComp->entity);
				m_SpriteRenderQueue.Draw(sprComp->sprite, sprComp->color, XMMatrixTranspose(trans.GetWorldMatrix()), trans.GetWorldPosition().z);
			}

			m_SpriteRenderQueue.End();
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
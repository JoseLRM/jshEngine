#include "Renderer2D.h"

//temp 
#include "Input.h"
#include "Window.h"

namespace jsh {

	bool Renderer2D::Initialize()
	{
		m_Rendering2DPass.Create();
		m_OutlineRenderPass.Create();
		m_PostProcessRenderPass.Create();

		m_RenderGraph.AddPass(&m_Rendering2DPass);
		m_RenderGraph.AddPass(&m_OutlineRenderPass);
		m_RenderGraph.AddPass(&m_PostProcessRenderPass);

		m_RenderGraph.Create();

		m_Rendering2DPass.SetSpriteBatch(&m_SpriteBatch);
		return true;
	}
	void Renderer2D::Begin()
	{
		jshGraphics::Begin();
	}
	void Renderer2D::Render()
	{
		CameraComponent* camera = jshScene::GetComponent<CameraComponent>(m_MainCamera);

		auto& spriteList = jshScene::_internal::GetComponentsList()[SpriteComponent::ID];
		m_SpriteBatch.Begin();

		for (uint32 i = 0; i < spriteList.size(); i += SpriteComponent::SIZE) {
			jsh::SpriteComponent* spriteComp = reinterpret_cast<SpriteComponent*>(&spriteList[i]);
			Transform& transform = jshScene::GetTransform(spriteComp->entity);
			m_SpriteBatch.Draw(transform.GetWorldMatrix(), spriteComp->color, spriteComp->sprite, transform.GetWorldPosition().z);
		}

		m_SpriteBatch.End();

		m_RenderGraph.Render(camera);
	}
	void Renderer2D::End()
	{
		jshGraphics::End();
		jshGraphics::Present(0u);
	}
	bool Renderer2D::Close()
	{
		return true;
	}

}
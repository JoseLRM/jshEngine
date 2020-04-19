#include "OutlineRenderPass.h"

#include "..//Renderer.h"
#include "..//..//components/OutlineComponent.h"
#include "PostProcess.h"

namespace jsh {

	OutlineRenderPass::OutlineRenderPass() {}

	void OutlineRenderPass::Create()
	{ 

		// RENDER TARGET VIEW
		{
			JSH_RENDER_TARGET_VIEW_DESC rtvDesc;
			rtvDesc.Format = JSH_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.Texture2D.MipSlice = 0u;
			rtvDesc.ViewDimension = JSH_RTV_DIMENSION_TEXTURE2D;

			JSH_TEXTURE2D_DESC resDesc;
			resDesc.ArraySize = 1u;
			resDesc.BindFlags = JSH_BIND_RENDER_TARGET | JSH_BIND_SHADER_RESOURCE;
			resDesc.CPUAccessFlags = 0u;
			resDesc.Format = JSH_FORMAT_R8G8B8A8_UNORM;
			resDesc.Width = 1080;
			resDesc.Height = 720;
			resDesc.MipLevels = 1u;
			resDesc.MiscFlags = 0u;
			resDesc.SampleDesc.Count = 1u;
			resDesc.SampleDesc.Quality = 0u;
			resDesc.Usage = JSH_USAGE_DEFAULT;

			jshGraphics::CreateRenderTargetView(&rtvDesc, &resDesc, &m_RenderTargetView);
		}

		// DEPTH STENCIL
		{
			JSH_DEPTH_STENCIL_DESC desc;
			jshZeroMemory(&desc, sizeof(JSH_DEPTH_STENCIL_DESC));
			desc.DepthEnable = false;
			desc.DepthFunc = JSH_COMPARISON_ALWAYS;
			desc.DepthWriteMask = JSH_DEPTH_WRITE_MASK_ZERO;

			desc.StencilEnable = true;
			desc.StencilReadMask = 0xFF;
			desc.StencilWriteMask = 0xFF;
			desc.FrontFace.StencilFailOp = JSH_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = JSH_STENCIL_OP_KEEP;
			desc.FrontFace.StencilPassOp = JSH_STENCIL_OP_REPLACE;
			desc.FrontFace.StencilFunc = JSH_COMPARISON_ALWAYS;
			desc.BackFace.StencilFailOp = JSH_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp = JSH_STENCIL_OP_KEEP;
			desc.BackFace.StencilPassOp = JSH_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc = JSH_COMPARISON_NEVER;

			jshGraphics::CreateDepthStencilState(&desc, &m_MaskDepthStencilState);
		}
		{
			JSH_DEPTH_STENCIL_DESC desc;
			jshZeroMemory(&desc, sizeof(JSH_DEPTH_STENCIL_DESC));
			desc.DepthEnable = false;
			desc.DepthFunc = JSH_COMPARISON_ALWAYS;
			desc.DepthWriteMask = JSH_DEPTH_WRITE_MASK_ZERO;

			desc.StencilEnable = true;
			desc.StencilReadMask = 0xFF;
			desc.StencilWriteMask = 0xFF;
			desc.FrontFace.StencilFailOp = JSH_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = JSH_STENCIL_OP_KEEP;
			desc.FrontFace.StencilPassOp = JSH_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = JSH_COMPARISON_NOT_EQUAL;
			desc.BackFace.StencilFailOp = JSH_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp = JSH_STENCIL_OP_KEEP;
			desc.BackFace.StencilPassOp = JSH_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc = JSH_COMPARISON_NEVER;

			jshGraphics::CreateDepthStencilState(&desc, &m_DrawDepthStencilState);
		}

		// CONSTANT BUFFER
		m_InstanceBuffer.Create();
		
		m_pShader = jshGraphics::GetShader("OutlineMask");

		m_BlurRenderPass.Create();
	}

	void OutlineRenderPass::Load() 
	{
		m_Instances.clear(false);
		
		auto& list = jshScene::_internal::GetComponentsList()[OutlineComponent::ID];
		for (uint32 i = 0; i < list.size(); i += OutlineComponent::SIZE) {

			OutlineComponent* outlineComp = reinterpret_cast<OutlineComponent*>(&list[i]);
			MeshComponent* meshComp = jshScene::GetComponent<MeshComponent>(outlineComp->entityID);

			if (meshComp == nullptr) continue;
			/*
				TODO: crear una lista en Lambertican pass de los objetos que aparecen en pantalla y utilizar estos para
				crear las instancias
			*/

			if (m_Instances.reserve_request()) m_Instances.reserve(5u);

			OutlineInstance& instance = m_Instances[m_Instances.size()];
			m_Instances.add_pos(1u);

			instance.meshComp = meshComp;
			instance.intensity = outlineComp->intensity;
			instance.color = outlineComp->color;

		}
	}

	void OutlineRenderPass::Render(CommandList cmd)
	{
		jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLES, cmd);
		jshGraphics::BindViewport(jshRenderer::primitives::GetDefaultViewport(), 0u, cmd);
		jshGraphics::BindConstantBuffer(jshRenderer::primitives::GetCameraBuffer(), JSH_GFX_SLOT_CBUFFER_CAMERA, JSH_SHADER_TYPE_VERTEX, cmd);
		m_InstanceBuffer.Bind(JSH_SHADER_TYPE_VERTEX, cmd);

		jshGraphics::ClearRenderTargetView(m_RenderTargetView, cmd);

		for (uint32 i = 0; i < m_Instances.size(); ++i) {
			OutlineInstance& instance = m_Instances[i];

			m_InstanceBuffer.UpdateBuffer(&XMMatrixTranspose(jshScene::GetTransform(instance.meshComp->entityID).GetWorldMatrix()), cmd);
			//m_VCBuffer.color = instance.color;
			//m_VCBuffer.intensity = instance.intensity;
			//jshGraphics::UpdateConstantBuffer(m_CBuffer, &m_VCBuffer, cmd);

			// mask
			jshGraphics::BindRenderTargetView(m_RenderTargetView, jshRenderer::primitives::GetDefaultDepthStencilView(), cmd);
			jshGraphics::BindDepthStencilState(m_MaskDepthStencilState, 1u, cmd);
			jshGraphics::BindInputLayout(m_pShader->inputLayout, cmd);
			jshGraphics::BindVertexShader(m_pShader->vs, cmd);
			jshGraphics::BindPixelShader(m_pShader->ps, cmd);
			instance.meshComp->mesh->GetRawData()->Bind(cmd);
			
			jshGraphics::DrawIndexed(instance.meshComp->mesh->GetRawData()->GetIndexCount(), cmd);

			// blur
			m_BlurRenderPass.Render(
				m_RenderTargetView, 
				jshRenderer::primitives::GetMainRenderTargetView(), 
				&m_DrawDepthStencilState,
				&jshRenderer::primitives::GetDefaultDepthStencilView(),
				1u,
				cmd);

		}

	}

}
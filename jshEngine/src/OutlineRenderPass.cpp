#include "common.h"
/*
#include "OutlineRenderPass.h"

#include "Renderer.h"
#include "OutlineComponent.h"
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
		{
			jsh::vec4 aux;
			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(jsh::vec4);
			desc.CPUAccessFlags = 0u;
			desc.MiscFlags = 0u;
			desc.StructureByteStride = 0u;
			desc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA sdata;
			sdata.pSysMem = &aux;
			jshGraphics::CreateBuffer(&desc, &sdata, &m_ColorBuffer);
		}
		m_InstanceBuffer.Create();
		
		m_pShader = jshGraphics::GetShader("OutlineMask");

		m_BlurRenderPass.Create();
	}

	void OutlineRenderPass::Load() 
	{
		auto& list = jshScene::_internal::GetComponentsList()[OutlineComponent::ID];

		m_Instances.clear();
		m_Instances.reserve(list.size() / OutlineComponent::SIZE);
		for (uint32 i = 0; i < list.size(); i += OutlineComponent::SIZE) {

			OutlineComponent* outlineComp = reinterpret_cast<OutlineComponent*>(&list[i]);
			MeshComponent* meshComp = jshScene::GetComponent<MeshComponent>(outlineComp->entity);

			if (meshComp == nullptr || meshComp->mesh == nullptr) continue;
			
				//TODO: crear una lista en Lambertican pass de los objetos que aparecen en pantalla y utilizar estos para
				//crear las instancias
				//TODO: inheritance outline
			

			m_Instances.emplace_back(OutlineInstance());
			OutlineInstance& instance = m_Instances[m_Instances.size()-1];

			instance.meshComp = meshComp;
			instance.mode = outlineComp->GetBlurMode();
			instance.radius = outlineComp->radius;
			instance.sigma = outlineComp->sigma;
			instance.color = outlineComp->color;

		}

		std::sort(m_Instances.begin(), m_Instances.end());
	}

	void OutlineRenderPass::Render(CommandList cmd)
	{
		if (m_Instances.empty()) return;

		Buffer& cameraBuffer = m_pRenderGraph->GetCameraBuffer();

		// MASK
		jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLES, cmd);
		jshGraphics::BindViewport(jshGraphics::primitives::GetDefaultViewport(), 0u, cmd);
		jshGraphics::BindConstantBuffer(cameraBuffer, JSH_GFX_SLOT_CBUFFER_CAMERA, JSH_SHADER_TYPE_VERTEX, cmd);
		m_InstanceBuffer.Bind(JSH_SHADER_TYPE_VERTEX, cmd);
		jshGraphics::BindRenderTargetView(m_RenderTargetView, jshGraphics::primitives::GetDefaultDepthStencilView(), cmd);
		jshGraphics::BindDepthStencilState(m_MaskDepthStencilState, 1u, cmd);
		jshGraphics::BindInputLayout(m_pShader->inputLayout, cmd);
		jshGraphics::BindVertexShader(m_pShader->vs, cmd);
		// TODO: Unbind pixelShader
		//jshGraphics::BindPixelShader(m_pShader->ps, cmd);
		
		for (uint32 i = 0; i < m_Instances.size(); ++i) {
			OutlineInstance& instance = m_Instances[i];

			m_InstanceBuffer.UpdateBuffer(&XMMatrixTranspose(jshScene::GetTransform(instance.meshComp->entity).GetWorldMatrix()), cmd);
			instance.meshComp->mesh->GetRawData()->Bind(cmd);

			jshGraphics::DrawIndexed(instance.meshComp->mesh->GetRawData()->GetIndexCount(), cmd);

		}

		// DRAW
		for (uint32 i = 0; i < m_Instances.size(); ++i) {
			uint32 radius = m_Instances[i].radius;
			uint32 mode = m_Instances[i].mode;
			float sigma = m_Instances[i].sigma;

			jshGraphics::UnbindTexture(0u, JSH_SHADER_TYPE_PIXEL, cmd);
			jshGraphics::BindRenderTargetView(m_RenderTargetView, jshGraphics::primitives::GetDefaultDepthStencilView(), cmd);
			jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLES, cmd);
			jshGraphics::BindViewport(jshGraphics::primitives::GetDefaultViewport(), 0u, cmd);
			jshGraphics::BindConstantBuffer(cameraBuffer, JSH_GFX_SLOT_CBUFFER_CAMERA, JSH_SHADER_TYPE_VERTEX, cmd);
			m_InstanceBuffer.Bind(JSH_SHADER_TYPE_VERTEX, cmd);
			jshGraphics::ClearRenderTargetView(m_RenderTargetView, 0.f, 0.f, 0.f, 0.f, cmd);
			jshGraphics::BindSamplerState(jshGraphics::primitives::GetDefaultSamplerState(), 0u, JSH_SHADER_TYPE_PIXEL, cmd);
			jshGraphics::BindDepthStencilState(m_MaskDepthStencilState, 1u, cmd);
			jshGraphics::BindInputLayout(m_pShader->inputLayout, cmd);
			jshGraphics::BindDepthStencilState(jshGraphics::primitives::GetDisabledDepthStencilState(), 0u, cmd);
			jshGraphics::BindVertexShader(m_pShader->vs, cmd);
			jshGraphics::BindPixelShader(m_pShader->ps, cmd);
			jshGraphics::BindConstantBuffer(m_ColorBuffer, 0u, JSH_SHADER_TYPE_PIXEL, cmd);
			jshGraphics::BindBlendState(jshGraphics::primitives::GetDefaultBlendState(), cmd);


			for (; i < m_Instances.size(); ++i) {
				OutlineInstance& instance = m_Instances[i];

				if (instance.radius != radius || instance.mode != mode || (mode == 0 && instance.sigma != sigma)) {
					i--;
					break;
				}

				m_InstanceBuffer.UpdateBuffer(&XMMatrixTranspose(jshScene::GetTransform(instance.meshComp->entity).GetWorldMatrix()), cmd);
				instance.meshComp->mesh->GetRawData()->Bind(cmd);

				// color buffer
				vec4 color = vec4(instance.color.x, instance.color.y, instance.color.z, 1.f);
				jshGraphics::UpdateBuffer(m_ColorBuffer, &color, 0u, cmd);

				jshGraphics::DrawIndexed(instance.meshComp->mesh->GetRawData()->GetIndexCount(), cmd);
			}

			// blur
			switch (mode)
			{
			case 0:
				m_BlurRenderPass.SetAlphaGaussianMode(radius, sigma);
				break;
			case 1:
				m_BlurRenderPass.SetAlphaBoxMode(radius);
				break;
			case 2:
				m_BlurRenderPass.SetSolidMode(radius);
				break;
			}

			m_BlurRenderPass.Render(
				m_RenderTargetView,
				jshGraphics::primitives::GetOffscreenRenderTargetView(),
				&m_DrawDepthStencilState,
				&jshGraphics::primitives::GetDefaultDepthStencilView(),
				1u,
				cmd);

		}
	}

}
*/
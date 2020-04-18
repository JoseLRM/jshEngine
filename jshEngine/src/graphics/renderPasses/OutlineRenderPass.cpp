#include "OutlineRenderPass.h"

#include "..//Renderer.h"
#include "..//..//components/OutlineComponent.h"

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
			desc.DepthWriteMask = JSH_DEPTH_WRITE_MASK_ALL;

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

		// CONSTANT BUFFER
		{
			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(m_VCBuffer);
			desc.CPUAccessFlags = 0u;
			desc.MiscFlags = 0u;
			desc.StructureByteStride = 0u;
			desc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA sdata;
			sdata.pSysMem = &m_VCBuffer;
			jshGraphics::CreateResource(&desc, &sdata, &m_CBuffer);
		}

		// SHADERS
		{
			jshGraphics::CreateVertexShader(L"OutlineMaskVertex.cso", &m_MaskVS);
			jshGraphics::CreatePixelShader(L"OutlineMaskPixel.cso", &m_MaskPS);
			const JSH_INPUT_ELEMENT_DESC desc[] = {
				{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0, true, 0u, 0u}
			};

			jshGraphics::CreateInputLayout(desc, 1u, m_MaskVS, &m_MaskIL);
		}

		m_System.AddRequestedComponents<MeshComponent>();
		m_System.AddRequestedComponents<OutlineComponent>();
		m_System.m_pInstances = &m_Instances;
	}

	void OutlineRenderPass::Load() 
	{
		m_Instances.clear(false);
		jshScene::UpdateSystem(&m_System, 0.f);
	}

	void OutlineRenderPass::Render()
	{
		jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLES, cmd);
		jshGraphics::BindViewport(jshRenderer::primitives::GetDefaultViewport(), 0u, cmd);
		jshGraphics::BindConstantBuffer(m_CBuffer, 0u, JSH_SHADER_TYPE_VERTEX, cmd);

		CameraComponent* camera = jshRenderer::GetMainCamera();
		m_VCBuffer.vm = camera->GetViewMatrix();
		m_VCBuffer.pm = camera->GetProjectionMatrix();

		for (uint32 i = 0; i < m_Instances.size(); ++i) {
			OutlineInstance& instance = m_Instances[i];

			m_VCBuffer.tm = XMMatrixTranspose(jshScene::GetTransform(instance.meshComp->entityID).GetWorldMatrix());
			//m_VCBuffer.color = instance.color;
			//m_VCBuffer.intensity = instance.intensity;
			jshGraphics::UpdateConstantBuffer(m_CBuffer, &m_VCBuffer, cmd);
			// mask
			jshGraphics::BindRenderTargetView(jshRenderer::primitives::GetOffscreenRenderTargetView(), jshRenderer::primitives::GetDefaultDepthStencilView(), cmd);
			jshGraphics::BindDepthStencilState(m_MaskDepthStencilState, 1u, cmd);
			instance.meshComp->mesh->GetRawData()->Bind(cmd);
			jshGraphics::BindInputLayout(m_MaskIL, cmd);
			jshGraphics::BindVertexShader(m_MaskVS, cmd);
			jshGraphics::BindPixelShader(m_MaskPS, cmd);
			
			jshGraphics::DrawIndexed(instance.meshComp->mesh->GetRawData()->GetIndexCount(), cmd);
		}
	}

	void OutlineSystem::UpdateEntity(Entity entity, BaseComponent** comp, float dt)
	{
		MeshComponent* meshComp = (MeshComponent*)comp[0];
		OutlineComponent* outline = (OutlineComponent*)comp[1];
		
		m_pInstances->push_back({ meshComp, outline->color, outline->intensity }, 10);
	}

}
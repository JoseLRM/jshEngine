#include "Rendering3DPass.h"

#include "Components.h"
#include "Engine.h"
#include "Renderer.h"
#include "PostProcess.h"
#include "BloomEffect.h"
#include "MeshBatch.h"

namespace jsh {

	Rendering3DPass::Rendering3DPass() {}

	void Rendering3DPass::Create()
	{
		{
			MeshData aux;
			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(MeshData);
			desc.CPUAccessFlags = 0u;
			desc.MiscFlags = 0u;
			desc.StructureByteStride = 0u;
			desc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA sdata;
			sdata.pSysMem = &aux;
			jshGraphics::CreateBuffer(&desc, &sdata, &m_MeshBuffer);
		}
		m_InstanceBuffer.Create();
		m_BlurEffect.Create();
	}

	void Rendering3DPass::Load()
	{		
		if (m_pMeshBatch == nullptr) return;

		// Sort instances
		auto& instances = m_pMeshBatch->GetInstances();
		std::sort(instances.begin(), instances.end());
	}

	void Rendering3DPass::Render(CommandList cmd)
	{
		if (m_pMeshBatch == nullptr) return;

		jsh::RenderTargetView& mainRenderTargetView = jshGraphics::primitives::GetMainRenderTargetView();
		jsh::RenderTargetView& offscreenRenderTargetView = jshGraphics::primitives::GetOffscreenRenderTargetView();
		jsh::DepthStencilState& depthStencilState = jshGraphics::primitives::GetDefaultDepthStencilState();
		jsh::TextureRes& dsResource = jshGraphics::primitives::GetDefaultDepthStencilView();
		SamplerState& st = jshGraphics::primitives::GetDefaultSamplerState();
		Viewport& vp = jshGraphics::primitives::GetDefaultViewport();

		// bind state
		jshGraphics::BindRenderTargetView(offscreenRenderTargetView, dsResource, cmd);
		jshGraphics::BindDepthStencilState(depthStencilState, 1u, cmd);
		
		jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLES, cmd);

		jshGraphics::BindSamplerState(st, 0u, JSH_SHADER_TYPE_PIXEL, cmd);
		jshGraphics::BindSamplerState(st, 1u, JSH_SHADER_TYPE_PIXEL, cmd);

		jshGraphics::BindBlendState(jshGraphics::primitives::GetDefaultBlendState(), cmd);

		jshGraphics::BindViewport(vp, 0u, cmd);

		Buffer& cameraBuffer = m_pRenderGraph->GetCameraBuffer();
		Buffer& lightBuffer = m_pRenderGraph->GetLightBuffer();

		jshGraphics::BindConstantBuffer(cameraBuffer, JSH_GFX_SLOT_CBUFFER_CAMERA, JSH_SHADER_TYPE_VERTEX, cmd);
		m_InstanceBuffer.Bind(JSH_SHADER_TYPE_VERTEX, cmd);
		jshGraphics::BindConstantBuffer(lightBuffer, JSH_GFX_SLOT_CBUFFER_LIGHTS, JSH_SHADER_TYPE_PIXEL, cmd);
		jshGraphics::BindConstantBuffer(m_MeshBuffer, 0, JSH_SHADER_TYPE_PIXEL, cmd);

		MeshData meshData;
		auto& instances = m_pMeshBatch->GetInstances();

		for (uint32 i = 0; i < instances.size(); ++i) {
			MeshInstance& instance = instances[i];

			Mesh* mesh = instance.mesh;
			if(mesh->IsTransparent()) jshGraphics::BindBlendState(jshGraphics::primitives::GetTransparentBlendState(), cmd);
			else jshGraphics::BindBlendState(jshGraphics::primitives::GetDefaultBlendState(), cmd);

			// update mesh data
			meshData.diffuseActived = mesh->HasDiffuseMap();
			meshData.normalActived = mesh->HasNormalMap();
			meshData.specularActived = mesh->HasSpecularMap();
			jshGraphics::UpdateBuffer(m_MeshBuffer, &meshData, 0u, cmd);

			// update instance data
			m_InstanceBuffer.UpdateBuffer(&XMMatrixTranspose(instance.pTransform->GetWorldMatrix()), cmd);

			mesh->Bind(cmd);
			if(mesh->GetMaterial() != nullptr)
				mesh->GetMaterial()->Bind(cmd, JSH_SHADER_TYPE_PIXEL);
			jshGraphics::DrawIndexed(mesh->GetRawData()->GetIndexCount(), cmd);
		}
	}

}
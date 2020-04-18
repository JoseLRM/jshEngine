#include "LambertianRenderPass.h"

#include "..//..//components/Components.h"
#include "..//..//Engine.h"
#include "..//Renderer.h"

namespace jsh {

	LambertianRenderPass::LambertianRenderPass() {}

	void LambertianRenderPass::Create()
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
			jshGraphics::CreateResource(&desc, &sdata, &m_MeshBuffer);
		}
		m_InstanceBuffer.Create();
	}
	void LambertianRenderPass::Load()
	{
		m_Instances.clear(false);
		
		auto meshList = jshScene::_internal::GetComponentsList()[MeshComponent::ID];
		for (uint32 i = 0; i < meshList.size(); i += MeshComponent::SIZE) {

			MeshComponent* meshComp = reinterpret_cast<MeshComponent*>(&meshList[i]);
			Transform& transform = jshScene::GetTransform(meshComp->entityID);

			assert(meshComp->mesh->GetShader() != nullptr && meshComp->mesh->GetRawData() != nullptr);

			// TODO: Frustum culling

			if (m_Instances.capacity() == m_Instances.size()) m_Instances.reserve(10u);
			MeshInstance& instance = m_Instances[m_Instances.size()];
			m_Instances.add_pos(1);

			instance.pTransform = &transform;
			instance.mesh = meshComp->mesh;
		}
	}

	void LambertianRenderPass::Render()
	{
		jsh::RenderTargetView& offscreenRenderTargetView = jshRenderer::primitives::GetOffscreenRenderTargetView();
		jsh::RenderTargetView& mainRenderTargetView = jshRenderer::primitives::GetMainRenderTargetView();
		jsh::DepthStencilState& depthStencilState = jshRenderer::primitives::GetDefaultDepthStencilState();
		jsh::Resource& dsResource = jshRenderer::primitives::GetDefaultDepthStencilView();
		SamplerState& st = jshRenderer::primitives::GetDefaultSamplerState();
		Viewport& vp = jshRenderer::primitives::GetDefaultViewport();

		// bind state
		jshGraphics::BindRenderTargetView(offscreenRenderTargetView, dsResource, cmd);
		jshGraphics::BindDepthStencilState(depthStencilState, 1u, cmd);
		
		jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLES, cmd);

		jshGraphics::BindSamplerState(st, 0u, JSH_SHADER_TYPE_PIXEL, cmd);
		jshGraphics::BindSamplerState(st, 1u, JSH_SHADER_TYPE_PIXEL, cmd);

		jshGraphics::BindViewport(vp, 0u, cmd);

		jshGraphics::BindConstantBuffer(jshRenderer::primitives::GetCameraBuffer(), JSH_GFX_SLOT_CBUFFER_CAMERA, JSH_SHADER_TYPE_VERTEX, cmd);
		m_InstanceBuffer.Bind(JSH_SHADER_TYPE_VERTEX, cmd);
		jshGraphics::BindConstantBuffer(jshRenderer::primitives::GetLightBuffer(), JSH_GFX_SLOT_CBUFFER_LIGHTS, JSH_SHADER_TYPE_PIXEL, cmd);
		jshGraphics::BindConstantBuffer(m_MeshBuffer, 0, JSH_SHADER_TYPE_PIXEL, cmd);

		MeshData meshData;

		for (uint32 i = 0; i < m_Instances.size(); ++i) {
			MeshInstance& instance = m_Instances[i];

			Mesh* mesh = instance.mesh;

			// update mesh data
			meshData.diffuseActived = mesh->HasTexture(JSH_TEXTURE_DIFFUSE_MAP);
			meshData.normalActived = mesh->HasTexture(JSH_TEXTURE_NORMAL_MAP);
			meshData.specularActived = mesh->HasTexture(JSH_TEXTURE_SPECULAR_MAP);
			jshGraphics::UpdateConstantBuffer(m_MeshBuffer, &meshData, cmd);

			// update instance data
			m_InstanceBuffer.UpdateBuffer(&XMMatrixTranspose(instance.pTransform->GetWorldMatrix()), cmd);

			mesh->Bind(cmd);
			mesh->GetMaterial()->Bind(cmd, JSH_SHADER_TYPE_PIXEL);
			jshGraphics::DrawIndexed(mesh->GetRawData()->GetIndexCount(), cmd);
		}
	}

}
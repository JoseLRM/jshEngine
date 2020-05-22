#include "common.h"

#include "MeshRenderer.h"
#include "Technique.h"

namespace jsh {

	MeshRenderer::MeshRenderer(RenderTargetView& rtv, TextureRes& dsv, Viewport& vp)
	{
		AddAttachment(reinterpret_cast<GfxPrimitive*>(&rtv));
		AddAttachment(reinterpret_cast<GfxPrimitive*>(&dsv));
		m_pViewport = &vp;
	}

	void MeshRenderer::Initialize()
	{
		SetForwardRendering();
	}
	void MeshRenderer::Update()
	{
		// TODO: Frustum Culling
		for (uint32 i = 0; i < m_Instances.size(); ++i) {
			m_InstancesID.push_back(i);
		}

		m_RenderingMode->Load(m_Instances, m_InstancesID, GetRenderGraph().GetLights());
	}
	void MeshRenderer::Render(CommandList cmd)
	{
		RenderTargetView& rtv = *reinterpret_cast<RenderTargetView*>(GetAttachment(0));
		TextureRes& dsv = *reinterpret_cast<TextureRes*>(GetAttachment(1));

		m_RenderingMode->Render(cmd, rtv, dsv, *m_pViewport, GetRenderGraph());
	}
	void MeshRenderer::Close()
	{
		m_RenderingMode->Close();
	}

	void MeshRenderer::DrawScene(Camera& camera)
	{
		m_Instances.clear();
		m_InstancesID.clear();
		XMMATRIX vm = camera.GetViewMatrix();
		
		auto& meshList = jshScene::_internal::GetComponentsList()[MeshComponent::ID];
		const uint32 componentSize = uint32(MeshComponent::SIZE);

		m_Instances.reserve(uint32(meshList.size()) / componentSize);

		for (uint32 i = 0; i < meshList.size(); i += componentSize) {

			MeshComponent* meshComp = reinterpret_cast<MeshComponent*>(&meshList[i]);
			Transform& transform = jshScene::GetTransform(meshComp->entity);

			if (meshComp->mesh == nullptr) continue;

			XMFLOAT4X4 m;
			XMStoreFloat4x4(&m, transform.GetWorldMatrix() * vm);
			m_Instances.emplace_back(meshComp->mesh->GetRawData(), meshComp->mesh->GetMaterial(), m);
		}
		
		std::sort(m_Instances.begin(), m_Instances.end());
	}

	void MeshRenderer::SetForwardRendering()
	{
		m_Mode = 0;
		m_RenderingMode = std::make_unique<_internal::ForwardRendering>();
		m_RenderingMode->Initialize();
	}
	void MeshRenderer::SetDeferredRendering()
	{
		m_Mode = 1;
		// TODO: Deferred Rendering
		jshFatalError("TODO: Deferred Rendering");
	}

	///////////////////////////FORWARD RENDERING///////////////////////////
	namespace _internal {

		void ForwardRendering::Initialize()
		{
			m_InstanceBuffer.Create(JSH_GFX_BATCH_COUNT * JSH_GFX_MESH_INSTANCE_SIZE);

			{
				JSH_BUFFER_DESC desc;
				desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
				desc.ByteWidth = JSH_GFX_MAX_LIGHTS * sizeof(LightBufferData) + sizeof(vec4);
				desc.CPUAccessFlags = JSH_CPU_ACCESS_WRITE;
				desc.MiscFlags = 0u;
				desc.StructureByteStride = 0u;
				desc.Usage = JSH_USAGE_DYNAMIC;
				jshGraphics::CreateBuffer(&desc, nullptr, &m_LightBuffer);
			}

			//BLEND STATE
			{
				JSH_BLEND_DESC desc;
				jshZeroMemory(&desc, sizeof(JSH_BLEND_DESC));
				desc.RenderTarget[0].BlendEnable = true;
				desc.RenderTarget[0].BlendOp = JSH_BLEND_OP_ADD;
				desc.RenderTarget[0].BlendOpAlpha = JSH_BLEND_OP_ADD;
				desc.RenderTarget[0].SrcBlend = JSH_BLEND_ONE;
				desc.RenderTarget[0].SrcBlendAlpha = JSH_BLEND_ONE;
				desc.RenderTarget[0].DestBlend = JSH_BLEND_ONE;
				desc.RenderTarget[0].DestBlendAlpha = JSH_BLEND_ZERO;
				desc.RenderTarget[0].RenderTargetWriteMask = JSH_COLOR_WRITE_ENABLE_ALL;

				jshGraphics::CreateBlendState(&desc, &m_AddColorBlendState);
			}
		}
		void ForwardRendering::Load(std::vector<MeshInstance>& instances, std::vector<uint32>& instancesIDs, std::vector<LightBufferData>& lights)
		{
			m_pInstances = &instances;
			m_pInstancesIDs = &instancesIDs;
		}
		void ForwardRendering::Render(CommandList cmd, RenderTargetView& rtv, TextureRes& dsv, Viewport& vp, RenderGraph& rg)
		{
			jshGraphics::BindRenderTargetView(rtv, dsv, cmd);
			jshGraphics::BindViewport(vp, 0u, cmd);

			for (uint32 i = 0; i < JSH_GFX_SAMPLER_STATES_COUNT; ++i) jshGraphics::BindSamplerState(jshGraphics::primitives::GetDefaultSamplerState(), i, JSH_SHADER_TYPE_PIXEL, cmd);

			// global buffers
			const Buffer* globalBuffer[] = {
				&rg.GetCameraBuffer()
			};
			jshGraphics::BindConstantBuffers(globalBuffer, JSH_GFX_SLOT_CBUFFER_CAMERA, 1u, JSH_SHADER_TYPE_VERTEX, cmd);

			globalBuffer[0] = &m_LightBuffer;
			jshGraphics::BindConstantBuffers(globalBuffer, JSH_GFX_SLOT_CBUFFER_LIGHTS, 1u, JSH_SHADER_TYPE_PIXEL, cmd);

			// draw
			auto& instances = *m_pInstances;
			auto& instancesIDs = *m_pInstancesIDs;

			uint32 lightsRendered = 0u;
			auto lights = rg.GetLights();

			jshGraphics::BindDepthStencilState(jshGraphics::primitives::GetDefaultDepthStencilState(), 1u, cmd);

			bool firstRender = true;

			while (lightsRendered < lights.size() || firstRender) {
				{
					jshZeroMemory(&m_LightData, JSH_GFX_MAX_LIGHTS * sizeof(LightBufferData) + sizeof(vec4));
					uint32 i = 0;
					while (i < JSH_GFX_MAX_LIGHTS) {
						uint32 j = i + lightsRendered;
						if (j == lights.size()) break;
						m_LightData.lights[i] = lights[j];
						++i;
					}
					lightsRendered += i;
				}

				if (firstRender) {
					m_LightData.ambient = vec4(jshRenderer::environment::GetAmbientLight());
					jshGraphics::BindBlendState(jshGraphics::primitives::GetDefaultBlendState(), cmd);
					firstRender = false;
				}
				else {
					m_LightData.ambient = { 0.f, 0.f, 0.f, 0.f };
					jshGraphics::BindBlendState(m_AddColorBlendState, cmd);
				}

				jshGraphics::UpdateBuffer(m_LightBuffer, &m_LightData, 0u, cmd);

				struct DrawCallData {
					uint32 beginIndex;
					uint32 endIndex;
					Material* material;
					RawData* rawData;
					DrawCallData(uint32 b, uint32 e, Material* m, RawData* rd) : beginIndex(b), endIndex(e), material(m), rawData(rd) {}
				};
				std::vector<DrawCallData> drawCallData;

				constexpr uint32 maxIndex = JSH_GFX_BATCH_COUNT * JSH_GFX_MESH_INSTANCE_SIZE;

				for (uint32 i = 0; i < instancesIDs.size(); ) {

					RawData* rawData = nullptr;
					Material* material = nullptr;

					MeshShader* shader = nullptr;

					uint32 indexCount = 0u;
					uint32 begin = 0;

					// crear datos por instancia y informacion para el renderizado
					for (; i < instancesIDs.size(); ++i) {
						auto& instance = instances[instancesIDs[i]];

						// allocate draw call data
						if (rawData != instance.rawData || material != instance.material) {
							// allocate draw call data
							if (indexCount > 0) {
								drawCallData.emplace_back(begin, indexCount, material, rawData);

								begin = indexCount;
							}

							rawData = instance.rawData;
							material = instance.material;
							shader = material->GetShader();
						}

						// comprobar si cambia de batch
						if (indexCount >= maxIndex) break;

						// fill instance data
						{
							XMMATRIX matrix = XMMatrixTranspose(XMLoadFloat4x4(&instance.tm));
							memcpy(&m_InstanceData[indexCount], &matrix, sizeof(XMMATRIX));
						}
						indexCount += JSH_GFX_MESH_INSTANCE_SIZE;
					}
					// set last draw data
					drawCallData.emplace_back(begin, indexCount, material, rawData);

					// update instance buffer
					m_InstanceBuffer.Update(m_InstanceData, indexCount, cmd);

					// draw calls
					shader = nullptr;

					for (uint32 j = 0; j < drawCallData.size(); ++j) {

						rawData = drawCallData[j].rawData;
						material = drawCallData[j].material;
						shader = material->GetShader();

						shader->Bind(rawData, cmd);
						shader->BindMaterialData(material->GetMaterialData(), cmd);

						jshGraphics::SetTopology(rawData->GetTopology(), cmd);
						Buffer& indexBuffer = rawData->GetIndexBuffer();
						if (indexBuffer.IsValid()) jshGraphics::BindIndexBuffer(indexBuffer, rawData->GetIndexBufferFormat(), 0u, cmd);

						const Buffer* buffers[] = {
							&rawData->GetVertexBuffer(),
							&m_InstanceBuffer.GetBuffer()
						};

						const uint32 strides[] = {
							jshGraphics::GetBufferDesc(*buffers[0]).StructureByteStride,
							JSH_GFX_MESH_INSTANCE_SIZE
						};

						const uint32 beginIndex = drawCallData[j].beginIndex;
						uint32 endIndex = drawCallData[j].endIndex;

						const uint32 offsets[] = {
							0u, beginIndex
						};

						jshGraphics::BindVertexBuffers(buffers, 0u, 2u, strides, offsets, cmd);
						rawData->Draw((endIndex - beginIndex) / strides[1], cmd);
					}

					drawCallData.clear();
				}
			}
		}
		void ForwardRendering::Close()
		{

		}

	}

}
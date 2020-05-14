#include "common.h"

#include "DrawRenderPass.h"

#include "Components.h"
#include "Renderer.h"
#include "PostProcess.h"
#include "BloomEffect.h"

namespace jsh {

	void DrawRenderPass::Create()
	{
		m_InstanceBuffer.Create(JSH_GFX_BATCH_COUNT * JSH_GFX_MAX_INSTANCE_SIZE);
	}

	void DrawRenderPass::Load()
	{		
		if (m_pRenderQueue == nullptr) return;
	}

	void DrawRenderPass::Render(CommandList cmd)
	{
		jshGraphics::BindRenderTargetView(jshGraphics::primitives::GetOffscreenRenderTargetView(), jshGraphics::primitives::GetDefaultDepthStencilView(), cmd);
		jshGraphics::BindViewport(jshGraphics::primitives::GetDefaultViewport(), 0u, cmd);

		for(uint32 i = 0; i < JSH_GFX_SAMPLER_STATES_COUNT; ++i) jshGraphics::BindSamplerState(jshGraphics::primitives::GetDefaultSamplerState(), i, JSH_SHADER_TYPE_PIXEL, cmd);

		// global buffers
		Buffer& cameraBuffer = m_pRenderGraph->GetCameraBuffer();
		Buffer& lightBuffer = m_pRenderGraph->GetLightBuffer();

		jshGraphics::BindConstantBuffers(&cameraBuffer, JSH_GFX_SLOT_CBUFFER_CAMERA, 1u, JSH_SHADER_TYPE_VERTEX, cmd);
		jshGraphics::BindConstantBuffers(&lightBuffer, JSH_GFX_SLOT_CBUFFER_LIGHTS, 1u, JSH_SHADER_TYPE_PIXEL, cmd);

		// draw
		if (m_pRenderQueue) {
			DrawMeshRenderQueue(m_pRenderQueue, cmd);
		}

		if (m_pSpriteRenderQueue) {
			DrawSpriteRenderQueue(m_pSpriteRenderQueue, cmd);
		}
		
	}

	void DrawRenderPass::DrawMeshRenderQueue(RenderQueue* renderQueue, CommandList cmd)
	{
		jshGraphics::BindDepthStencilState(jshGraphics::primitives::GetDefaultDepthStencilState(), 1u, cmd);
		jshGraphics::BindBlendState(jshGraphics::primitives::GetDefaultBlendState(), cmd);

		auto& instances = renderQueue->GetInstances();

		struct DrawCallData {
			uint32 beginIndex;
			uint32 endIndex;
			uint32 stride;
			Material* material;
			RawData* rawData;
			InputLayout il;
			DrawCallData(uint32 b, uint32 e, uint32 s, Material* m, RawData* rd, InputLayout il) : beginIndex(b), endIndex(e), stride(s), material(m), rawData(rd), il(il) {}
		};
		std::vector<DrawCallData> drawCallData;

		for (uint32 i = 0; i < instances.size(); ) {

			RawData* rawData = nullptr;
			Material* material = nullptr;
			InputLayout il;

			Shader* shader = nullptr;

			uint32 instanceSize = 0u;

			uint32 indexCount = 0u;
			constexpr uint32 maxIndex = JSH_GFX_BATCH_COUNT * JSH_GFX_MAX_INSTANCE_SIZE;
			uint32 begin = 0;

			// crear datos por instancia y informacion para el renderizado
			for (; i < instances.size(); ++i) {
				auto& instance = instances[i];

				// allocate draw call data
				if (rawData != instance.rawData || material != instance.material) {
					// allocate draw call data
					if (indexCount > 0) {
						drawCallData.emplace_back(begin, indexCount, instanceSize, material, rawData, il);

						begin = indexCount;
					}

					rawData = instance.rawData;
					material = instance.material;
					il = instance.il;
					shader = material->GetShader();
					instanceSize = shader->GetInstanceSize();
				}

				// comprobar si cambia de batch
				if (indexCount >= maxIndex) break;

				// fill instance data
				shader->SetInstanceData(&m_InstanceData[indexCount], instance.trans);
				indexCount += instanceSize;
			}
			// set last draw data
			drawCallData.emplace_back(begin, indexCount, instanceSize, material, rawData, il);

			// update instance buffer
			m_InstanceBuffer.Update(m_InstanceData, indexCount, cmd);

			// draw calls
			shader = nullptr;
			for (uint32 j = 0; j < drawCallData.size(); ++j) {

				rawData = drawCallData[j].rawData;
				il = drawCallData[j].il;
				material = drawCallData[j].material;
				if (shader != material->GetShader()) {
					shader = material->GetShader();
					shader->Bind(cmd);
				}

				// bind constantData
				shader->BindConstantData(*material->GetConstantData(), cmd);

				jshGraphics::BindInputLayout(il, cmd);
				jshGraphics::SetTopology(rawData->GetTopology(), cmd);
				Buffer indexBuffer = rawData->GetIndexBuffer();
				if (indexBuffer.IsValid()) jshGraphics::BindIndexBuffer(indexBuffer, rawData->GetIndexBufferFormat(), 0u, cmd);

				Buffer buffers[] = {
					rawData->GetVertexBuffer(),
					m_InstanceBuffer.GetBuffer()
				};

				const uint32 beginIndex = drawCallData[j].beginIndex;
				uint32 endIndex = drawCallData[j].endIndex;

				const uint32 strides[] = {
					jshGraphics::GetBufferDesc(buffers[0]).StructureByteStride,
					drawCallData[j].stride
				};

				const uint32 offsets[] = {
					0u, beginIndex
				};

				jshGraphics::BindVertexBuffers(buffers, 0u, 2u, strides, offsets, cmd);
				rawData->Draw((endIndex - beginIndex) / strides[1], cmd);
			}

			drawCallData.clear();
		}
	}

	void DrawRenderPass::DrawSpriteRenderQueue(SpriteRenderQueue* renderQueue, CommandList cmd)
	{
		auto& instances = renderQueue->GetInstances();

		jshGraphics::BindBlendState(jshGraphics::primitives::GetTransparentBlendState(), cmd);
		jshGraphics::BindDepthStencilState(jshGraphics::primitives::GetDisabledDepthStencilState(), 0u, cmd);

		RawData* spriteRawData = jshGraphics::objects::GetSpriteRawData();
		Shader* shader = renderQueue->GetShader();
		shader->Bind(cmd);
		jshGraphics::BindInputLayout(renderQueue->GetInputLayout(), cmd);
		jshGraphics::SetTopology(spriteRawData->GetTopology(), cmd);

		const uint32 strides[] = {
					jshGraphics::GetBufferDesc(spriteRawData->GetVertexBuffer()).StructureByteStride,
					shader->GetInstanceSize()
		};

		Buffer buffers[] = {
					spriteRawData->GetVertexBuffer(),
					m_InstanceBuffer.GetBuffer()
		};

		struct DrawCallData {
			uint32 beginIndex;
			uint32 endIndex;
			Texture* texture;

			DrawCallData(uint32 begin, uint32 end, Texture* tex) : beginIndex(begin), endIndex(end), texture(tex) {}
		};
		std::vector<DrawCallData> drawCallData;

		for (uint32 i = 0; i < instances.size();) {

			Texture* texture = instances[i].sprite.texture;
			uint32 begin = 0u;
			uint32 indexCount = 0u;

			const uint32 maxIndex = JSH_GFX_BATCH_COUNT * JSH_GFX_MAX_INSTANCE_SIZE - shader->GetInstanceSize();

			for (; i < instances.size(); ++i) {

				if (indexCount >= maxIndex) break;

				auto& instance = instances[i];
				if (texture != instance.sprite.texture) {
					drawCallData.emplace_back(begin, indexCount, texture);

					begin = indexCount;
					texture = instance.sprite.texture;
				}

				shader->SetInstanceData(m_InstanceData + indexCount, instance.tm, instance.sprite, instance.color);
				indexCount += shader->GetInstanceSize();

			}
			drawCallData.emplace_back(begin, indexCount, texture);

			m_InstanceBuffer.Update(m_InstanceData, indexCount, cmd);

			for (uint32 j = 0; j < drawCallData.size(); ++j) {
				texture = drawCallData[j].texture;

				uint32 beginIndex = drawCallData[j].beginIndex;
				uint32 endIndex = drawCallData[j].endIndex;

				if (texture) {
					jshGraphics::BindTexture(texture->resource, JSH_GFX_SLOT_TEXTURE2D_FREE0, JSH_SHADER_TYPE_PIXEL, cmd);
					jshGraphics::BindSamplerState(texture->samplerState, JSH_GFX_SLOT_SAMPLER_FREE0, JSH_SHADER_TYPE_PIXEL, cmd);
				}

				const uint32 offsets[] = {
					0u, beginIndex
				};

				jshGraphics::BindVertexBuffers(buffers, 0u, 2, strides, offsets, cmd);

				spriteRawData->Draw((endIndex - beginIndex) / shader->GetInstanceSize(), cmd);
			}
			drawCallData.clear();
		}
	}

}
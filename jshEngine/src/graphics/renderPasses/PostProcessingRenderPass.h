#pragma once

#include "RenderPass.h"

namespace jsh {

	class PostProcessingRenderPass : public RenderPass {

		Buffer m_vBuffer;
		Buffer m_iBuffer;
		Buffer m_cBuffer;
		InputLayout m_InputLayout;

	public:
		PostProcessingRenderPass() {}

		void Create() override
		{
			float vData[] = {
				-1.f, 1.f,
				 1.f, 1.f,
				-1.f,-1.f,
				 1.f,-1.f
			};

			uint32 iData[] = {
				0, 1, 2, 
				2, 1, 3
			};

			alignas(16) float cData = 1.f;

			JSH_BUFFER_DESC vDesc;
			vDesc.BindFlags = JSH_BIND_VERTEX_BUFFER;
			vDesc.ByteWidth = sizeof(float) * 8;
			vDesc.CPUAccessFlags = 0u;
			vDesc.MiscFlags = 0u;
			vDesc.StructureByteStride = 2 * sizeof(float);
			vDesc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA vSubData;
			vSubData.pSysMem = &vData;
			jshGraphics::CreateBuffer(&vDesc, &vSubData, &m_vBuffer);

			JSH_BUFFER_DESC iDesc;
			iDesc.BindFlags = JSH_BIND_INDEX_BUFFER;
			iDesc.ByteWidth = sizeof(uint32) * 6;
			iDesc.CPUAccessFlags = 0u;
			iDesc.MiscFlags = 0u;
			iDesc.StructureByteStride = sizeof(uint32);
			iDesc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA iSubData;
			iSubData.pSysMem = &iData;
			jshGraphics::CreateBuffer(&iDesc, &iSubData, &m_iBuffer);

			JSH_BUFFER_DESC cDesc;
			cDesc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			cDesc.ByteWidth = sizeof(float)*4;
			cDesc.CPUAccessFlags = 0u;
			cDesc.MiscFlags = 0u;
			cDesc.StructureByteStride = 0;
			cDesc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA cSubData;
			cSubData.pSysMem = &cData;
			jshGraphics::CreateBuffer(&cDesc, &cSubData, &m_cBuffer);

			const JSH_INPUT_ELEMENT_DESC ilDesc[] = {
				{"Position", 0, JSH_FORMAT_R32G32_FLOAT, 0, true, 0, 0}
			};

			VertexShader* vs = (VertexShader*)jshGraphics::Get("PostProcessVertex");
			jshGraphics::CreateInputLayout(ilDesc, 1, *vs, &m_InputLayout);
			
		}

	protected:
		void Render() override
		{
			FrameBuffer* mainFB = (FrameBuffer*)jshGraphics::Get("MainFrameBuffer");
			SamplerState* sampler = (SamplerState*)jshGraphics::Get("DefaultSamplerState");
			Viewport* vp = (Viewport*)jshGraphics::Get("DefaultViewport");
			jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLES, cmd);

			mainFB->Bind(cmd);
			jshGraphics::BindSamplerState(*sampler, 0, JSH_SHADER_TYPE_PIXEL, cmd);
			jshGraphics::BindViewport(*vp, 0, cmd);

			jshGraphics::BindVertexBuffer(m_vBuffer, 0u, cmd);
			jshGraphics::BindIndexBuffer(m_iBuffer, cmd);
			jshGraphics::BindConstantBuffer(m_cBuffer, 0u, JSH_SHADER_TYPE_PIXEL, cmd);
			jshGraphics::BindInputLayout(m_InputLayout, cmd);
			VertexShader* vs = (VertexShader*)jshGraphics::Get("PostProcessVertex");
			PixelShader* ps = (PixelShader*)jshGraphics::Get("BlackAndWhitePP");
			jshGraphics::BindVertexShader(*vs, cmd);
			jshGraphics::BindPixelShader(*ps, cmd);

			//jshGraphics::DrawIndexed(6, cmd);
		}

	};

}
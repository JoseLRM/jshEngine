#pragma once

#include "RenderGraph.h"
#include "BlurEffect.h"
#include "Scene.h"

namespace jsh {

	class OutlineRenderPass : public RenderPass {
		
		struct OutlineInstance {
			MeshComponent* meshComp;
			jsh::vec3 color;
			uint32 mode;
			uint32 radius;
			float sigma;

			bool operator<(const OutlineInstance& other)
			{
				if (other.radius == radius) {
					if (other.mode == mode) {
						if(mode) return meshComp < other.meshComp;
						if (other.sigma == sigma) return meshComp < other.meshComp;
						else return other.sigma < sigma;
					}
					else return other.mode < mode;
				}
				return other.radius > radius;
			}
		};

		std::vector<OutlineInstance> m_Instances;

		RenderTargetView m_RenderTargetView;
		DepthStencilState m_MaskDepthStencilState;
		DepthStencilState m_DrawDepthStencilState;
		Buffer m_ColorBuffer;

		Shader* m_pShader;

		InstanceBuffer m_InstanceBuffer;

		BlurEffect m_BlurRenderPass;

	public:
		OutlineRenderPass();

		void Load() override;
		void Create() override;
		void Render(CommandList cmd) override;

	};

}
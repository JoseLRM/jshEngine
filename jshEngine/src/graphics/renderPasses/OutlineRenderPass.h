#pragma once

#include "..//RenderGraph.h"
#include "BlurEffect.h"
#include "..//..//ecs/Scene.h"

namespace jsh {

	class OutlineRenderPass : public RenderPass {
		
		struct OutlineInstance {
			MeshComponent* meshComp;
			jsh::vec3 color;
			uint32 intensity;
		};

		jsh::vector<OutlineInstance> m_Instances;

		RenderTargetView m_RenderTargetView;
		DepthStencilState m_MaskDepthStencilState;
		DepthStencilState m_DrawDepthStencilState;

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
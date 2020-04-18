#pragma once

#include "..//RenderGraph.h"
#include "..//..//ecs/Scene.h"

namespace jsh {

	struct OutlineInstance {
		MeshComponent* meshComp;
		jsh::vec3 color;
		uint32 intensity;
	};

	class OutlineSystem : public System {
	public:
		jsh::vector<OutlineInstance>* m_pInstances;
		void UpdateEntity(Entity entity, BaseComponent** comp, float dt) override;
	};

	class OutlineRenderPass : public RenderPass {
		OutlineSystem m_System;

		RenderTargetView m_RenderTargetView;
		DepthStencilState m_MaskDepthStencilState;
		Resource m_CBuffer;

		VertexShader m_MaskVS;
		PixelShader m_MaskPS;
		InputLayout m_MaskIL;

		struct alignas(16) {
			XMMATRIX tm;
			XMMATRIX vm;
			XMMATRIX pm;

			//jsh::vec3 color;
			//uint32 intensity;
		} m_VCBuffer;

		jsh::vector<OutlineInstance> m_Instances;

	public:
		OutlineRenderPass();

		void Load() override;
		void Create() override;
		void Render() override;

	};

}
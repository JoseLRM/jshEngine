#pragma once

#include "..//RenderGraph.h"

namespace jsh {

	class LambertianRenderPass : public RenderPass {

		struct alignas(16) MeshData {
			BOOL diffuseActived;
			BOOL normalActived;
			BOOL specularActived;
		};

		struct MeshInstance {
			Mesh* mesh;
			Transform* pTransform;
		};

		jsh::vector<MeshInstance> m_Instances;

		Resource m_MeshBuffer;
		InstanceBuffer m_InstanceBuffer;

	public:
		LambertianRenderPass();

		void Load() override;
		void Create() override;
		void Render() override;

	};

}
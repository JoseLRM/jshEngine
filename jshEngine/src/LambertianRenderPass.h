#pragma once

#include "RenderGraph.h"
#include "BlurEffect.h"

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

			bool operator<(const MeshInstance& other)
			{
				if (mesh->IsTransparent()) {
					if (other.mesh->IsTransparent()) return mesh < other.mesh;
					else return true;
				}
				else {
					if (other.mesh->IsTransparent()) return false;
					else return mesh < other.mesh;
				}
			}
		};

		std::vector<MeshInstance> m_Instances;

		Resource m_MeshBuffer;
		InstanceBuffer m_InstanceBuffer;
		BlurEffect m_BlurEffect;

	public:
		LambertianRenderPass();

		void Load() override;
		void Create() override;
		void Render(CommandList cmd) override;

	};

}
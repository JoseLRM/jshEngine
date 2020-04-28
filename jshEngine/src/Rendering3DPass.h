#pragma once

#include "RenderGraph.h"
#include "BlurEffect.h"

namespace jsh {

	class MeshBatch;
	struct CameraComponent;

	class Rendering3DPass : public RenderPass {

		struct alignas(16) MeshData {
			BOOL diffuseActived;
			BOOL normalActived;
			BOOL specularActived;
		};

		Buffer m_MeshBuffer;
		InstanceBuffer m_InstanceBuffer;
		BlurEffect m_BlurEffect;

		MeshBatch* m_pMeshBatch = nullptr;
		CameraComponent* m_pCamera = nullptr;

	public:
		Rendering3DPass();

		inline void SetMeshBatch(MeshBatch* pMeshBatch) noexcept { m_pMeshBatch = pMeshBatch; }
		inline void SetCamera(CameraComponent* pCamera) noexcept { m_pCamera = pCamera; }

		void Load() override;
		void Create() override;
		void Render(CommandList cmd) override;

	};

}
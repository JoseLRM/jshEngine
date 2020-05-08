#pragma once

#include "RenderGraph.h"
#include "RenderQueue.h"
#include "SpriteRenderQueue.h"

namespace jsh {

	class MeshBatch;
	struct CameraComponent;

	class DrawRenderPass : public RenderPass {

		RenderQueue* m_pRenderQueue = nullptr;
		SpriteRenderQueue* m_pSpriteRenderQueue = nullptr;

		byte m_InstanceData[JSH_GFX_BATCH_COUNT * JSH_GFX_MAX_INSTANCE_SIZE];
		InstanceBuffer m_InstanceBuffer;

	public:
		inline void SetRenderQueue(RenderQueue* rq) noexcept { m_pRenderQueue = rq; }
		inline void SetSpriteRenderQueue(SpriteRenderQueue* rq) noexcept { m_pSpriteRenderQueue = rq; }

		void Load() override;
		void Create() override;
		void Render(CommandList cmd) override;

	private:
		void DrawMeshRenderQueue(RenderQueue* renderQueue, CommandList cmd);
		void DrawSpriteRenderQueue(SpriteRenderQueue* renderQueue, CommandList cmd);

	};

}
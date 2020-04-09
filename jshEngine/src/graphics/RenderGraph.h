#pragma once

#include "renderPasses/RenderPass.h"

namespace jsh {

	class RenderGraph {
		jsh::vector<RenderPass*> m_RenderPasses;

	public:
		RenderGraph();
		~RenderGraph();

		virtual void Initialize() = 0;
		void Render();

	protected:
		inline void AddPass(RenderPass* pass) noexcept { m_RenderPasses.push_back(pass); }

	};

}
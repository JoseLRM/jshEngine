#pragma once

#include "Graphics.h"

namespace jsh {

	class RenderPass;

	class RenderGraph {
		jsh::vector<RenderPass*> m_RenderPasses;

		bool m_Modified = true;

	public:
		RenderGraph();
		~RenderGraph();

		void Render();

		inline void AddPass(RenderPass* pass) noexcept 
		{ 
			m_RenderPasses.push_back(pass);
			m_Modified = true;
		}

	private:
		void UpdateGraph();
	};

	class RenderPass {
	protected:
		jsh::vector<RenderPass*> m_Depencences;

	public:
		friend RenderGraph;
		RenderPass() {}

		virtual void Create() = 0;
		virtual void Load() {}
		virtual void Render(CommandList cmd) = 0;

		void AddDependence(RenderPass* renderPass) noexcept;

	};

}
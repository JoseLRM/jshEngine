#pragma once

#include "..//Graphics.h"

namespace jsh {

	class RenderGraph;

	class RenderPass {
	protected:
		jsh::CommandList cmd;

	public:
		friend RenderGraph;
		RenderPass();

		virtual void Create() = 0;
		void Run();

	protected:
		virtual void Render() = 0;

	};

}
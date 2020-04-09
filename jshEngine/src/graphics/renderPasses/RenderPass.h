#pragma once

#include "..//Graphics.h"

namespace jsh {

	class RenderPass {
	protected:
		jsh::CommandList cmd;

	public:
		RenderPass();

		virtual void Create() = 0;
		void Run();

	protected:
		virtual void Render() = 0;

	};

}
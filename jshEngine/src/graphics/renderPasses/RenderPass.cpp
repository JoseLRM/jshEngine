#include "RenderPass.h"

namespace jsh {

	RenderPass::RenderPass() : cmd(0) {}

	void RenderPass::Run()
	{
		Render();
	}

}
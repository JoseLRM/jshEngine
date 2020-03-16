#pragma once

#include "GraphicsDesc.h"

namespace jshGraphics {

#ifdef JSH_ENGINE
	bool Initialize();
	bool Close();

	void Prepare();
#endif

	void SetClearScreenColor(float c = 0.f);
	void SetClearScreenColor(float r, float g, float b);

	void SetTopology(JSH_TOPOLOGY topology);

}
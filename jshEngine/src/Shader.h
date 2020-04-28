#pragma once

#include "GraphicsPrimitives.h"

namespace jsh {

	class Shader {
	public:
		VertexShader vs;
		PixelShader ps;

		InputLayout inputLayout;
#ifndef JSH_ENGINE
	private:
#endif
		Shader() = default;
	};

}
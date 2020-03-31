#pragma once

#include "..//common.h"#include "..//graphics/GraphicsObjects.h"

namespace jshLoader {

	jsh::Model* LoadModel(const char* path);
	jsh::Texture LoadTexture(const char* path, jsh::Sampler sampler = jsh::INVALID_SAMPLER);

}
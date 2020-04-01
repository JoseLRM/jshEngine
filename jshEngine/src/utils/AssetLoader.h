#pragma once

#include "..//common.h"#include "..//graphics/GraphicsObjects.h"

namespace jshLoader {

	jsh::Model* LoadModel(const char* path, const char* name);
	jsh::Texture LoadTexture(const char* path);

}
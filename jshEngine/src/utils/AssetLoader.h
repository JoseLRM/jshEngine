#pragma once

#include "..//common.h"#include "..//graphics/GraphicsObjects.h"

namespace jshLoader {

	bool LoadModel(const char* path, jsh::Model* model);
	bool LoadTexture(const char* path, jsh::Texture* texture);

}
#pragma once

#include "common.h"#include "GraphicsObjects.h"

namespace jshLoader {

	bool LoadModel(const char* path, const char* name, jsh::Model* model);
	bool LoadTexture(const char* path, jsh::TextureRes* texture);

}
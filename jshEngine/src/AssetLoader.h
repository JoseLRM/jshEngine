#pragma once

#include "common.h"

namespace jshLoader {

	std::shared_ptr<jsh::Model> LoadModel(const char* path, const char* name);
	bool LoadTexture(const char* path, jsh::Resource* texture);

}
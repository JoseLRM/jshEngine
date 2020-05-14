#pragma once
#include "GraphicsObjects.h"

namespace jsh {
	class Image;
}

namespace jshLoader {

	bool LoadModel(const char* path, const char* name, jsh::Model* model);
	bool LoadTexture(const char* path, jsh::TextureRes* texture);

	void CreateTexture(const jsh::Image& image, jsh::TextureRes* tex, bool dynamic);

}
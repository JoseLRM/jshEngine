#pragma once

#include "..//ecs/Scene.h"
#include "..//graphics/GraphicsObjects.h"

namespace jsh {

	struct MeshComponent : public Component<MeshComponent> {

		Mesh mesh;

		MeshComponent() : mesh() {}
		MeshComponent(const Mesh& mesh) : mesh(mesh) {}
	};
	jshDefineComponent(MeshComponent);

}
#pragma once

#include "..//..//ecs/Scene.h"
#include "..//..//graphics/GraphicsObjects.h"

namespace jsh {

	struct ModelComponent : public Component<ModelComponent> {

		Model* model;

		ModelComponent() : model() {}
		ModelComponent(Model* model) : model(model) {}
	};
	jshDefineComponent(ModelComponent);
	struct MeshComponent : public Component<MeshComponent> {

		Mesh* mesh;

		MeshComponent() : mesh() {}
		MeshComponent(Mesh* model) : mesh(model) {}
	};
	jshDefineComponent(MeshComponent);

}
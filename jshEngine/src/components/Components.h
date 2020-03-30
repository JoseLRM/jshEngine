#pragma once

#include "..//ecs/Component.h"
#include "..//graphics/GraphicsObjects.h"

namespace jsh {

	struct NameComponent : jsh::Component<NameComponent> {
		const char* name;

		NameComponent(const char* name) : name(name) {}
	};
	jshDefineComponent(NameComponent);

	///////////////////////////////////////////// 3D ////////////////////////////////////

	struct Transform3DComponent : jsh::Component<Transform3DComponent> {

		vec3 position;
		vec3 size;
		vec3 rotation;

		Transform3DComponent() : position(), size(1.f, 1.f, 1.f), rotation() {}
		Transform3DComponent(float x, float y, float z) : position(x, y, z), size(1.f, 1.f, 1.f), rotation() {}
		Transform3DComponent(float x, float y, float z, float w, float h, float p) : position(x, y, z), size(w, h, p), rotation() {}

#ifdef JSH_IMGUI
		void ShowInfo() override
		{
			ImGui::DragFloat3("Position", &position.x, 1.f);
			ImGui::DragFloat3("Size", &size.x, 1.f);
			ImGui::DragFloat3("Rotation", &rotation.x, 1.f);
		}
#endif
	};
	jshDefineComponent(Transform3DComponent);

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

	///////////////////////////////////////////// 2D ////////////////////////////////////

	struct Transform2DComponent : jsh::Component<Transform2DComponent> {

		vec2 position;
		vec2 size;
		float rotation;

		Transform2DComponent() : position(), size(1.f, 1.f), rotation(0.f) {}
		Transform2DComponent(float x, float y) : position(x, y), size(1.f, 1.f), rotation(0.f) {}
		Transform2DComponent(float x, float y, float w, float h) : position(x, y), size(w, h), rotation(0.f) {}

#ifdef JSH_IMGUI
		void ShowInfo() override
		{
			ImGui::DragFloat2("Position", &position.x, 1.f);
			ImGui::DragFloat2("Size", &size.x, 1.f);
			ImGui::DragFloat("Rotation", &rotation, 1.f);
		}
#endif
	};
	jshDefineComponent(Transform2DComponent);

}
#pragma once

#include "..//..//ecs/Scene.h"

namespace jsh {

	struct Transform3DComponent : jsh::Component<Transform3DComponent> {

		vec3 size;
		vec3 position;
		vec3 rotation;

		Transform3DComponent() : position(), size(1.f, 1.f, 1.f), rotation() {}
		Transform3DComponent(float x, float y, float z) : position(x, y, z), size(1.f, 1.f, 1.f), rotation() {}
		Transform3DComponent(float x, float y, float z, float w, float h, float p) : position(x, y, z), size(w, h, p), rotation() {}

#ifdef JSH_IMGUI
		void ShowInfo() override
		{
			ImGui::DragFloat3("Position", &position.x, 1.f);
			ImGui::DragFloat3("Rotation", &rotation.x, 1.f);
			ImGui::DragFloat3("Size", &size.x, 1.f);
		}
#endif
	};
	jshDefineComponent(Transform3DComponent);

}
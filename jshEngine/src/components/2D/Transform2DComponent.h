#pragma once

#include "..//..//ecs/Scene.h"

namespace jsh {

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
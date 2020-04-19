#pragma once

#include "Scene.h"

namespace jsh {

	struct OutlineComponent : public Component<OutlineComponent> {

		vec3 color;
		uint32 intensity;

#ifdef JSH_IMGUI
		void ShowInfo() override
		{
			ImGui::ColorPicker4("Outline Color", &color.x);
		}
#endif

	};
	jshDefineComponent(OutlineComponent);

}
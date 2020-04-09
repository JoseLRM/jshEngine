#pragma once

#include "..//ecs/Scene.h"

namespace jsh {

	struct LightComponent : public Component<LightComponent> {

		float quadraticAttenuation = 0.025f;
		float constantAttenuation = 1.f;
		float intensity = 5.f;
		uint32 lightType = 0;

		vec4 color = { 1.f, 1.f, 1.f, 1.f };

#ifdef JSH_IMGUI
		void ShowInfo() override
		{
			ImGui::DragFloat("Quadratic Att", &quadraticAttenuation, 0.005f, 0.0001f, FLT_MAX);
			ImGui::DragFloat("Constant Att", &constantAttenuation, 0.075f, 0.0001f, FLT_MAX);
			ImGui::DragFloat("Intensity", &intensity, 0.2f, 0.0001f, FLT_MAX);
			ImGui::ColorPicker3("Color", &color.x);
		}
#endif

	};
	jshDefineComponent(LightComponent);

}
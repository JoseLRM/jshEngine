#pragma once

#include "Scene.h"
#include "GraphicsDesc.h"

namespace jsh {

	struct LightComponent : public Component<LightComponent> {

		float quadraticAttenuation = 0.025f;
		float constantAttenuation = 1.f;
		float intensity = 5.f;
		JSH_LIGHT_TYPE lightType = JSH_LIGHT_TYPE_POINT;

		vec4 color = { 1.f, 1.f, 1.f, 1.f };
		float spotRange = 0.3f;

#ifdef JSH_IMGUI
		void ShowInfo() override
		{
			const char* lightTypeName;
			switch (lightType)
			{
			case JSH_LIGHT_TYPE_POINT:
				lightTypeName = "Point Light";
				break;
			case JSH_LIGHT_TYPE_DIRECTION:
				lightTypeName = "Direction Light";
				break;
			case JSH_LIGHT_TYPE_SPOT:
				lightTypeName = "Spot Light";
				break;
			default:
				lightTypeName = "Invalid Light";
				break;
			}

			if (ImGui::BeginCombo("Light Type", lightTypeName)) {

				if (ImGui::Button("Invalid Light")) lightType = JSH_LIGHT_TYPE_INVALID;
				if (ImGui::Button("Point Light")) lightType = JSH_LIGHT_TYPE_POINT;
				if (ImGui::Button("Direction Light")) lightType = JSH_LIGHT_TYPE_DIRECTION;
				if (ImGui::Button("Spot Light")) lightType = JSH_LIGHT_TYPE_SPOT;

				ImGui::EndCombo();
			}

			if (lightType == JSH_LIGHT_TYPE_SPOT) {
				ImGui::SliderFloat("Spot Range", &spotRange, 0.f, 1.f);
			}

			ImGui::DragFloat("Quadratic Att", &quadraticAttenuation, 0.0005f, 0.0001f, FLT_MAX);
			ImGui::DragFloat("Constant Att", &constantAttenuation, 0.075f, 0.0001f, FLT_MAX);
			ImGui::DragFloat("Intensity", &intensity, 0.2f, 0.0001f, FLT_MAX);
			ImGui::ColorPicker3("Color", &color.x);
		}
#endif

	};
	jshDefineComponent(LightComponent);

}
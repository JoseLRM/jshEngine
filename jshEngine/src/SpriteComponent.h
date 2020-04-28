#pragma once

#include "Scene.h"

namespace jsh {

	struct SpriteComponent : public jsh::Component<SpriteComponent> {
		Sprite sprite;
		vec4 color;

#ifdef JSH_IMGUI
		void ShowInfo() override 
		{
			if (ImGui::BeginCombo("Select Texture", "Select Texture")) {
				jsh::Texture* tex = jshGraphics::GetTextureImGui();
				if (tex != nullptr) sprite.texture = tex;
				ImGui::EndCombo();
			}
			ImGui::Text("Sprite Coords");
			ImGui::SliderFloat2("Position", &sprite.coords.x, 0.f, 1.f);
			ImGui::SliderFloat2("Dimension", &sprite.coords.z, 0.f, 1.f);

			ImGui::ColorPicker4("Color", &color.x);
		}
#endif
	};
	jshDefineComponent(SpriteComponent);

}
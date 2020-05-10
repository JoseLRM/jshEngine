#pragma once

#include "Scene.h"

namespace jsh {

	struct SpriteComponent : public jsh::Component<SpriteComponent> {
		Sprite sprite;
		Color color = jshColor::WHITE;

		SpriteComponent() = default;
		SpriteComponent(const Sprite& sprite) : sprite(sprite) {}
		SpriteComponent(const Color& color) : color(color) {}
		SpriteComponent(const Sprite& sprite, const Color& color) : sprite(sprite), color(color) {}

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

			float c[4];
			c[0] = (float)(color.x) / 255.f;
			c[1] = (float)(color.y) / 255.f;
			c[2] = (float)(color.z) / 255.f;
			c[3] = (float)(color.w) / 255.f;
			ImGui::ColorPicker4("Color", c);
			color.x = byte(c[0] * 255.f);
			color.y = byte(c[1] * 255.f);
			color.z = byte(c[2] * 255.f);
			color.w = byte(c[3] * 255.f);
		}
#endif
	};
	jshDefineComponent(SpriteComponent);

}
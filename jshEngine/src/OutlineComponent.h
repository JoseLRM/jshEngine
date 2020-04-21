#pragma once

#include "Scene.h"

namespace jsh {

	struct OutlineComponent : public Component<OutlineComponent> {

		vec3 color = { 1.f, 1.f, 1.f };
		uint32 radius = 4;
		float sigma = 1.f;

	private:
		uint8 m_BlurMode;

	public:
		inline void SetGaussianMode() noexcept { m_BlurMode = 0; }
		inline void SetBoxMode() noexcept { m_BlurMode = 1; }

		inline bool InGaussianMode() const noexcept { return m_BlurMode == 0; }
		inline bool InBoxMode() const noexcept { return m_BlurMode == 1; }

#ifdef JSH_IMGUI
		void ShowInfo() override
		{
			if (ImGui::BeginCombo("Blur Mode", m_BlurMode ? "Box" : "Gaussian")) {
				if (ImGui::Button("Gaussian")) m_BlurMode = 0;
				if (ImGui::Button("Box")) m_BlurMode = 1;
				ImGui::EndCombo();
			}

			int r = radius;
			ImGui::SliderInt("Radius", &r, 0, 16);
			radius = r;
			if (m_BlurMode == 0) ImGui::SliderFloat("Sigma", &sigma, 0.f, 10.f);

			ImGui::ColorPicker4("Outline Color", &color.x);
		}
#endif

	};
	jshDefineComponent(OutlineComponent);

}
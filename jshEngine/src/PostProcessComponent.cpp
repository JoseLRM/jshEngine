#include "common.h"
#include "PostProcessComponent.h"

namespace jsh {

	void PostProcessComponent::SetBloomEffect(bool enable, uint32 radius, float sigma, float intensity) noexcept {

		if (radius > 16) radius = 16;
		if (intensity < 0.f) intensity = 0.f;
		else if (intensity > 1.f) intensity = 1.f;

		m_BloomEffect = enable;
		m_BloomRadius = radius;
		m_BloomSigma = 20.f;
		m_BloomIntensity = intensity;

	}

#ifdef JSH_IMGUI
	void PostProcessComponent::ShowInfo()
	{
		ImGui::Checkbox("Bloom Effect", &m_BloomEffect);
		if (m_BloomEffect) {
			int r = m_BloomRadius;
			ImGui::SliderInt("Bloom Radius", &r, 1, 16);
			m_BloomRadius = r;
			ImGui::SliderFloat("BloomSigma", &m_BloomSigma, 0.f, 30.f);
			ImGui::SliderFloat("BloomIntensity", &m_BloomIntensity, 0.f, 1.f);
		}

		ImGui::DragFloat("Contrast", &m_Contrast, 0.01f);
		ImGui::DragFloat("Brightness", &m_Brightness, 0.01f);

	}
#endif

}
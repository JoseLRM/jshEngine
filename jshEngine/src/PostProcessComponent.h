#pragma once

#include "Scene.h"

namespace jsh {

	struct PostProcessComponent : public jsh::Component<PostProcessComponent> {
	private:
		// Bloom effect
		bool m_BloomEffect = false;
		uint32 m_BloomRadius = 16.f;
		float m_BloomSigma = 20.f;
		float m_BloomIntensity = 0.5f;

		// Color Correction
		float m_Contrast = 0.3f;
		float m_Brightness = 1.f;

	public:
		PostProcessComponent() {}

		void SetBloomEffect(bool enable, uint32 radius, float sigma, float intensity) noexcept;
		inline void SetBloomEffect(bool enable) noexcept { m_BloomEffect = enable; }
		inline bool IsBloomEffect() const noexcept { return m_BloomEffect; }
		inline uint32 GetBloomRadius() const noexcept { return m_BloomRadius; }
		inline float GetBloomSigma() const noexcept { return m_BloomSigma; }
		inline float GetBloomIntensity() const noexcept { return m_BloomIntensity; }

		inline void SetContrast(float contrast) noexcept { m_Contrast = contrast; }
		inline void SetBrightness(float brightness) noexcept { m_Brightness = brightness; }
		inline float GetContrast() const noexcept { return m_Contrast; }
		inline float GetBrightness() const noexcept { return m_Brightness; }

#ifdef JSH_IMGUI
		void ShowInfo() override;
#endif

	};
	jshDefineComponent(PostProcessComponent);

}
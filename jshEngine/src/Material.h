#pragma once

#include "GraphicsPrimitives.h"

namespace jsh {

	class Material {
		bool m_Modified = true;
		float m_SpecularIntensity = 1.f;
		float m_Shininess = 5.f;
		Buffer m_Buffer;
	public:
		void Create();
		void Bind(jsh::CommandList cmd, JSH_SHADER_TYPE shaderType);

		inline float GetSpecularIntensity() const noexcept { return m_SpecularIntensity; }
		inline float GetShininess() const noexcept { return m_Shininess; }

		inline void SetSpecularIntensity(float si) noexcept
		{
			m_SpecularIntensity = si;
			m_Modified = true;
		}
		inline void SetShininess(float s) noexcept
		{
			m_Shininess = s;
			m_Modified = true;
		}

#ifndef JSH_ENGINE
	private:
#endif
		Material() = default;
	};


}
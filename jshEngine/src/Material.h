#pragma once

#include "GraphicsPrimitives.h"
#include "Shader.h"

namespace jsh {
	
	class Material {
	protected:
		Shader* m_pShader;
		ConstantData m_ConstantData;

	public:

		void SetShader(Shader* shader) {
			m_pShader = shader;
			m_ConstantData = shader->CreateConstantData();
		}

		inline ConstantData* GetConstantData() noexcept { return &m_ConstantData; }
		inline Shader* GetShader() noexcept { return m_pShader; }
	};


}
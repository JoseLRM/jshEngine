#include "Graphics.h"

namespace jsh {

	void Mesh::Draw(uint32 instances, jsh::CommandList cmd)
	{
		m_RawData->Draw(instances, cmd);
	}

	void Mesh::SetRawData(RawData* rawData) noexcept
	{
		assert(rawData != nullptr);
		m_RawData = rawData;

		if (m_Material) UpdateInputLayout();
	}
	void Mesh::SetMaterial(Material* material) noexcept
	{
		assert(material != nullptr);
		m_Material = material;

		if (m_RawData) UpdateInputLayout();
	}

	void Mesh::UpdateInputLayout() noexcept
	{
		Shader* shader = m_Material->GetShader();
		if (!shader) return;

		m_Valid = jshGraphics::GenerateInputLayout(shader, m_RawData, &m_InputLayout);
	}

}
#include "GraphicsObjects.h"

namespace jsh {

	void Mesh::Bind(CommandList cmd)
	{
		m_RawData->Bind(cmd);

		if (m_DiffuseMapEnabled)	jshGraphics::BindTexture(m_DiffuseMap->resource, JSH_GFX_SLOT_TEXTURE2D_DIFFUSE, JSH_SHADER_TYPE_PIXEL, cmd);
		if (m_NormalMapEnabled)		jshGraphics::BindTexture(m_NormalMap->resource, JSH_GFX_SLOT_TEXTURE2D_NORMAL, JSH_SHADER_TYPE_PIXEL, cmd);
		if (m_SpecularMapEnabled)	jshGraphics::BindTexture(m_SpecularMap->resource, JSH_GFX_SLOT_TEXTURE2D_SPECULAR, JSH_SHADER_TYPE_PIXEL, cmd);

		jshGraphics::BindInputLayout(m_Shader->inputLayout, cmd);
		jshGraphics::BindVertexShader(m_Shader->vs, cmd);
		jshGraphics::BindPixelShader(m_Shader->ps, cmd);
	}

	void Mesh::SetDiffuseMap(Texture* texture) noexcept
	{
		
		if (texture->resource.IsValid()) {
			m_DiffuseMap = texture;
			m_DiffuseMapEnabled = true;
			m_Modified = true;
		}
	}
	void Mesh::SetNormalMap(Texture* texture) noexcept
	{
		if (texture->resource.IsValid()) {
			m_NormalMap = texture;
			m_NormalMapEnabled = true;
			m_Modified = true;
		}
	}
	void Mesh::SetSpecularMap(Texture* texture) noexcept
	{
		if (texture->resource.IsValid()) {
			m_SpecularMap = texture;
			m_SpecularMapEnabled = true;
			m_Modified = true;
		}
	}
	void Mesh::EnableDiffuseMap(bool enable) noexcept
	{
		if (enable && m_DiffuseMap) m_DiffuseMapEnabled = true;
		else m_DiffuseMapEnabled = false;
		m_Modified = true;
	}
	void Mesh::EnableNormalMap(bool enable) noexcept
	{
		if (enable && m_NormalMap) m_NormalMapEnabled = true;
		else m_NormalMapEnabled = false;
		m_Modified = true;
	}
	void Mesh::EnableSpecularMap(bool enable) noexcept
	{
		if (enable && m_SpecularMap) m_SpecularMapEnabled = true;
		else m_SpecularMapEnabled = false;
		m_Modified = true;
	}

	void Mesh::SetRawData(RawData* rawData) noexcept
	{
		assert(rawData != nullptr);
		m_RawData = rawData;
		m_Modified = true;
	}
	void Mesh::SetMaterial(Material* material) noexcept
	{
		assert(material != nullptr);
		m_Material = material;
	}
	void Mesh::SetShader(Shader* shader) noexcept
	{
		assert(shader != nullptr);
		m_Shader = shader;
		m_Modified = true;
	}

	void Mesh::UpdatePrimitives() noexcept
	{
		if (m_Modified) {

			m_Modified = false;

			RawDataFlags flags = m_RawData->GetFlags();

			if (flags & JSH_RAW_DATA_TEX_COORDS && flags & JSH_RAW_DATA_TANGENTS && (m_SpecularMapEnabled || m_NormalMapEnabled)) {
				m_Shader = jshGraphics::GetShader("Normal");
			}
			else if (flags & JSH_RAW_DATA_TEX_COORDS && m_DiffuseMapEnabled) {
				m_Shader = jshGraphics::GetShader("SimpleTex");
			}
			else if (flags & JSH_RAW_DATA_COLORS) {
				m_Shader = jshGraphics::GetShader("SimpleCol");
			}
			else {
				m_Shader = jshGraphics::GetShader("Solid");
			}
		}
	}

}
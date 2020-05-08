#include "Shader.h"

#include "Transform.h"
#include "GraphicsObjects.h"

namespace jsh {

	void Shader::Bind(jsh::CommandList cmd)
	{
		if (m_VS.IsValid()) jshGraphics::BindVertexShader(m_VS, cmd);
		if (m_PS.IsValid()) jshGraphics::BindPixelShader(m_PS, cmd);
		//if (m_GS.IsValid()) jshGraphics::BindGeometryShader(m_GS, cmd);
	}

	void Shader::SetInstanceData(byte* data, Transform* trans)
	{
		XMMATRIX matrix = XMMatrixTranspose(trans->GetWorldMatrix());
		memcpy(data, &matrix, sizeof(XMMATRIX));
	}

	void Shader::SetInstanceData(byte* data, const XMMATRIX& tm, const Sprite& sprite, const Color& color)
	{
		memcpy(data, &tm, sizeof(XMMATRIX));
		memcpy(data + sizeof(XMMATRIX), &sprite.coords, sizeof(vec4));
		memcpy(data + sizeof(XMMATRIX) + sizeof(vec4), &color, sizeof(Color));
		BOOL texture = sprite.texture ? TRUE : FALSE;
		memcpy(data + sizeof(XMMATRIX) + sizeof(vec4) + sizeof(Color), &texture, sizeof(BOOL));
	}

	///////////////////////////SOLID SHADER////////////////////////////////////
	ConstantData SolidShader::CreateConstantData()
	{
		ConstantData cd;

		// Buffer
		{
			cd.constantBuffers.emplace_back();
			float shininess = 1.f;
			float specularIntensity = 1.f;
			vec3 color = { 1.f, 1.f, 1.f };

			cd.constantBuffers[0].AddValue("Shininess", &shininess, sizeof(float));
			cd.constantBuffers[0].AddValue("SpecularIntensity", &specularIntensity, sizeof(float));
			cd.constantBuffers[0].AddValue("Color", &color.x, sizeof(vec3));

			cd.constantBuffers[0].Create(0u, JSH_USAGE_DEFAULT, 0u);
		}

		return cd;
	}

	void SolidShader::BindConstantData(ConstantData& cd, CommandList cmd)
	{
		jshGraphics::BindConstantBuffers(&cd.constantBuffers[0].Get(cmd), JSH_GFX_SLOT_CBUFFER_FREE0, 1, JSH_SHADER_TYPE_PIXEL, cmd);
	}

#ifdef JSH_IMGUI
	void SolidShader::ShowConstantDataInfo(ConstantData& cd)
	{
		ImGui::DragFloat("Shininess", reinterpret_cast<float*>(cd.constantBuffers[0].GetValue("Shininess")), 0.03f, 0.01f, 1000.f);
		ImGui::DragFloat("Spec Intensity", reinterpret_cast<float*>(cd.constantBuffers[0].GetValue("SpecularIntensity")), 0.03f, 0.01f, 1000.f);
		ImGui::ColorPicker3("Color", reinterpret_cast<float*>(cd.constantBuffers[0].GetValue("Color")));
		cd.constantBuffers[0].Update();
	}
#endif

	////////////////////////////NORMAL SHADER///////////////////////////////////////
	ConstantData NormalShader::CreateConstantData()
	{
		ConstantData cd;

		// constant buffer
		{
			cd.constantBuffers.emplace_back();
			BOOL diffuseMap = FALSE;
			BOOL normalMap = FALSE;
			BOOL specularMap = FALSE;

			float shininess = 1.f;
			float specularIntensity = 1.f;

			cd.constantBuffers[0].AddValue("DiffuseMap", &diffuseMap, sizeof(BOOL));
			cd.constantBuffers[0].AddValue("NormalMap", &normalMap, sizeof(BOOL));
			cd.constantBuffers[0].AddValue("SpecularMap", &specularMap, sizeof(BOOL));
			
			cd.constantBuffers[0].AddValue("Shininess", &shininess, sizeof(float));
			cd.constantBuffers[0].AddValue("SpecularIntensity", &specularIntensity, sizeof(float));

			cd.constantBuffers[0].Create();
		}

		return cd;
	}

	void NormalShader::BindConstantData(ConstantData& cd, CommandList cmd)
	{
		jshGraphics::BindConstantBuffers(&cd.constantBuffers[0].Get(cmd), JSH_GFX_SLOT_CBUFFER_FREE0, 1u, JSH_SHADER_TYPE_PIXEL, cmd);
		
		if(cd.textures[0].IsValid()) jshGraphics::BindTexture(cd.textures[0], JSH_GFX_SLOT_TEXTURE2D_FREE0, JSH_SHADER_TYPE_PIXEL, cmd);
		if(cd.textures[1].IsValid()) jshGraphics::BindTexture(cd.textures[1], JSH_GFX_SLOT_TEXTURE2D_FREE0 + 1, JSH_SHADER_TYPE_PIXEL, cmd);
		if(cd.textures[2].IsValid()) jshGraphics::BindTexture(cd.textures[2], JSH_GFX_SLOT_TEXTURE2D_FREE0 + 2, JSH_SHADER_TYPE_PIXEL, cmd);
	}

	void NormalShader::SetDiffuseMap(Texture* texture, ConstantData& cd)
	{
		BOOL enable;
		if (texture) {
			cd.textures[0] = texture->resource;
			cd.samplers[0] = texture->samplerState;
			enable = TRUE;
		}
		else {
			cd.textures[0] = TextureRes();
			enable = FALSE;
		}
		cd.constantBuffers[0].SetValue("DiffuseMap", &enable);
	}

	void NormalShader::SetNormalMap(Texture* texture, ConstantData& cd)
	{
		BOOL enable;
		if (texture) {
			cd.textures[1] = texture->resource;
			enable = TRUE;
		}
		else {
			cd.textures[1] = TextureRes();
			enable = FALSE;
		}
		cd.constantBuffers[0].SetValue("NormalMap", &enable);
	}

	void NormalShader::SetSpecularMap(Texture* texture, ConstantData& cd)
	{
		BOOL enable;
		if (texture) {
			cd.textures[2] = texture->resource;
			enable = TRUE;
		}
		else {
			cd.textures[2] = TextureRes();
			enable = FALSE;
		}
		cd.constantBuffers[0].SetValue("SpecularMap", &enable);
	}

#ifdef JSH_IMGUI
	void NormalShader::ShowConstantDataInfo(ConstantData& cd)
	{
		{
			if (ImGui::BeginCombo("DiffuseMap", "DiffuseMap")) {
				Texture* tex = jshGraphics::GetTextureImGui();
				if (tex != nullptr) SetDiffuseMap(tex, cd);
				ImGui::EndCombo();
			}
			bool diffuse = *(BOOL*)cd.constantBuffers[0].GetValue("DiffuseMap");
			ImGui::Checkbox("Diffuse Enabled", &diffuse);
			BOOL d = diffuse;
			cd.constantBuffers[0].SetValue("DiffuseMap", &d);
		}
		{
			if (ImGui::BeginCombo("NormalMap", "NormalMap")) {
				Texture* tex = jshGraphics::GetTextureImGui();
				if (tex != nullptr) SetNormalMap(tex, cd);
				ImGui::EndCombo();
			}
			bool normal = *(BOOL*)cd.constantBuffers[0].GetValue("NormalMap");
			ImGui::Checkbox("Normal Enabled", &normal);
			BOOL d = normal;
			cd.constantBuffers[0].SetValue("NormalMap", &d);
		}
		{
			if (ImGui::BeginCombo("SpecularMap", "SpecularMap")) {
				Texture* tex = jshGraphics::GetTextureImGui();
				if (tex != nullptr) SetSpecularMap(tex, cd);
				ImGui::EndCombo();
			}
			bool specular = *(BOOL*)cd.constantBuffers[0].GetValue("SpecularMap");
			ImGui::Checkbox("Specular Enabled", &specular);
			BOOL d = specular;
			cd.constantBuffers[0].SetValue("SpecularMap", &d);
		}

		ImGui::DragFloat("Shininess", reinterpret_cast<float*>(cd.constantBuffers[0].GetValue("Shininess")), 0.03f, 0.01f, 1000.f);
		ImGui::DragFloat("Spec Intensity", reinterpret_cast<float*>(cd.constantBuffers[0].GetValue("SpecularIntensity")), 0.03f, 0.01f, 1000.f);
		
		cd.constantBuffers[0].Update();
	}
#endif

}
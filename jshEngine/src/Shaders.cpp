#include "common.h"
#include "Shaders.h"

#include "Transform.h"
#include "GraphicsObjects.h"

namespace jsh {

	///////////////////////////SOLID SHADER////////////////////////////////////
	void SolidShader::Create()
	{
		jshGraphics::CreateVertexShader(L"SolidVertex.cso", &m_VertexShader);
		jshGraphics::CreatePixelShader(L"SolidPixel.cso", &m_PixelShader);

		AddVertexProperty("Position", JSH_FORMAT_R32G32B32_FLOAT, 0u);
		AddVertexProperty("Normal", JSH_FORMAT_R32G32B32_FLOAT, 0u);
	}

	void* SolidShader::CreateMaterialData()
	{
		MaterialData* matData = new MaterialData();

		// Buffer
		{
			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(matData->mat);
			desc.CPUAccessFlags = 0u;
			desc.MiscFlags = 0u;
			desc.StructureByteStride = 0u;
			desc.Usage = JSH_USAGE_DEFAULT;

			JSH_SUBRESOURCE_DATA sdata;
			sdata.pSysMem = &matData->mat;

			jshGraphics::CreateBuffer(&desc, &sdata, &matData->buffer);
		}

		return matData;
	}

	void SolidShader::DestroyMaterialData(void* matData)
	{
		MaterialData& materialData = *reinterpret_cast<MaterialData*>(matData);
		materialData.buffer.Release();
		delete matData;
	}

	void SolidShader::BindMaterialData(void* matData, CommandList cmd)
	{
		MaterialData& materialData = *reinterpret_cast<MaterialData*>(matData);

		if (materialData.update) {
			jshGraphics::UpdateBuffer(materialData.buffer, &materialData.mat, 0u, cmd);
			materialData.update = false;
		}

		const Buffer* buffers[] = {
			&materialData.buffer
		};
		jshGraphics::BindConstantBuffers(buffers, JSH_GFX_SLOT_CBUFFER_FREE0, 1, JSH_SHADER_TYPE_PIXEL, cmd);
	}

	void SolidShader::SetShininess(float n, void* matData)
	{
		MaterialData& materialData = *reinterpret_cast<MaterialData*>(matData);
		materialData.mat.shininess = n;
		materialData.update = true;
	}
	void SolidShader::SetSpecularIntensity(float n, void* matData)
	{
		MaterialData& materialData = *reinterpret_cast<MaterialData*>(matData);
		materialData.mat.specIntensity = n;
		materialData.update = true;
	}

#ifdef JSH_IMGUI
	void SolidShader::ShowMaterialInfo(void* matData)
	{
		MaterialData& materialData = *reinterpret_cast<MaterialData*>(matData);

		ImGui::DragFloat("Shininess", &materialData.mat.shininess, 0.03f, 0.01f, 1000.f);
		ImGui::DragFloat("Spec Intensity", &materialData.mat.specIntensity, 0.03f, 0.01f, 1000.f);
		ImGui::ColorPicker3("Color", &materialData.mat.color.x);
		materialData.update = true;
	}
#endif

	////////////////////////////NORMAL SHADER///////////////////////////////////////
	void NormalShader::Create()
	{
		jshGraphics::CreateVertexShader(L"NormalVertex.cso", &m_VertexShader);
		jshGraphics::CreatePixelShader(L"NormalPixel.cso", &m_PixelShader);

		AddVertexProperty("Position", JSH_FORMAT_R32G32B32_FLOAT, 0u);
		AddVertexProperty("Normal", JSH_FORMAT_R32G32B32_FLOAT, 0u);
		AddVertexProperty("TexCoord", JSH_FORMAT_R32G32_FLOAT, 0u);
		AddVertexProperty("Tangent", JSH_FORMAT_R32G32B32_FLOAT, 0u);
		AddVertexProperty("Bitangent", JSH_FORMAT_R32G32B32_FLOAT, 0u);
	}

	void* NormalShader::CreateMaterialData()
	{
		MaterialData& materialData = *new MaterialData();

		materialData.sampler = &jshGraphics::primitives::GetDefaultSamplerState();
		materialData.diffuseSampler = &jshGraphics::primitives::GetDefaultSamplerState();

		// Buffer
		{
			JSH_BUFFER_DESC desc;
			desc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(materialData.mat);
			desc.CPUAccessFlags = 0u;
			desc.MiscFlags = 0u;
			desc.StructureByteStride = 0u;
			desc.Usage = JSH_USAGE_DEFAULT;

			JSH_SUBRESOURCE_DATA sdata;
			sdata.pSysMem = &materialData.mat;

			jshGraphics::CreateBuffer(&desc, &sdata, &materialData.buffer);
		}

		return &materialData;
	}

	void NormalShader::DestroyMaterialData(void* matData)
	{
		MaterialData& materialData = *reinterpret_cast<MaterialData*>(matData);
		materialData.buffer.Release();
		delete matData;
	}

	void NormalShader::BindMaterialData(void* matData, CommandList cmd)
	{
		MaterialData& materialData = *reinterpret_cast<MaterialData*>(matData);

		if (materialData.update) {
			jshGraphics::UpdateBuffer(materialData.buffer, &materialData.mat, 0u, cmd);
			materialData.update = false;
		}

		const Buffer* buffers[] = {
			&materialData.buffer
		};
		jshGraphics::BindConstantBuffers(buffers, JSH_GFX_SLOT_CBUFFER_FREE0, 1u, JSH_SHADER_TYPE_PIXEL, cmd);
		
		if(materialData.diffuseMap)		jshGraphics::BindTexture(*materialData.diffuseMap, JSH_GFX_SLOT_TEXTURE2D_FREE0, JSH_SHADER_TYPE_PIXEL, cmd);
		if(materialData.normalMap)		jshGraphics::BindTexture(*materialData.normalMap, JSH_GFX_SLOT_TEXTURE2D_FREE0 + 1, JSH_SHADER_TYPE_PIXEL, cmd);
		if(materialData.specularMap)	jshGraphics::BindTexture(*materialData.specularMap, JSH_GFX_SLOT_TEXTURE2D_FREE0 + 2, JSH_SHADER_TYPE_PIXEL, cmd);

		if (materialData.diffuseSampler) jshGraphics::BindSamplerState(*materialData.diffuseSampler, 0u, JSH_SHADER_TYPE_PIXEL, cmd);
		jshGraphics::BindSamplerState(*materialData.sampler, 1u, JSH_SHADER_TYPE_PIXEL, cmd);
	}

	void NormalShader::SetShininess(float n, void* matData)
	{
		MaterialData& materialData = *reinterpret_cast<MaterialData*>(matData);
		materialData.mat.shininess = n;
		materialData.update = true;
	}
	void NormalShader::SetSpecularIntensity(float n, void* matData)
	{
		MaterialData& materialData = *reinterpret_cast<MaterialData*>(matData);
		materialData.mat.specIntensity = n;
		materialData.update = true;
	}

	void NormalShader::SetDiffuseMap(Texture* texture, void* matData)
	{
		MaterialData& materialData = *reinterpret_cast<MaterialData*>(matData);
		BOOL enable;
		if (texture) {
			materialData.diffuseMap = &texture->resource;
			materialData.diffuseSampler = texture->samplerState;
			enable = TRUE;
		}
		else {
			materialData.diffuseMap = nullptr;
			materialData.diffuseSampler = nullptr;
			enable = FALSE;
		}
		materialData.mat.hasDiffuse = enable;
		materialData.update = true;
	}

	void NormalShader::SetNormalMap(Texture* texture, void* matData)
	{
		MaterialData& materialData = *reinterpret_cast<MaterialData*>(matData);
		BOOL enable;
		if (texture) {
			materialData.normalMap = &texture->resource;
			enable = TRUE;
		}
		else {
			materialData.normalMap = nullptr;
			enable = FALSE;
		}
		materialData.mat.hasNormal = enable;
		materialData.update = true;
	}

	void NormalShader::SetSpecularMap(Texture* texture, void* matData)
	{
		MaterialData& materialData = *reinterpret_cast<MaterialData*>(matData);
		BOOL enable;
		if (texture) {
			materialData.specularMap = &texture->resource;
			enable = TRUE;
		}
		else {
			materialData.specularMap = nullptr;
			enable = FALSE;
		}
		materialData.mat.hasSpecular = enable;
		materialData.update = true;
	}

#ifdef JSH_IMGUI
	void NormalShader::ShowMaterialInfo(void* matData)
	{
		MaterialData& materialData = *reinterpret_cast<MaterialData*>(matData);
		{
			if (ImGui::BeginCombo("DiffuseMap", "DiffuseMap")) {
				Texture* tex = jshGraphics::GetTextureImGui();
				if (tex != nullptr) SetDiffuseMap(tex, matData);
				ImGui::EndCombo();
			}
			bool diffuse = materialData.mat.hasDiffuse;
			ImGui::Checkbox("Diffuse Enabled", &diffuse);
			materialData.mat.hasDiffuse = diffuse;
		}
		{
			if (ImGui::BeginCombo("NormalMap", "NormalMap")) {
				Texture* tex = jshGraphics::GetTextureImGui();
				if (tex != nullptr) SetNormalMap(tex, matData);
				ImGui::EndCombo();
			}
			bool normal = materialData.mat.hasNormal;
			ImGui::Checkbox("Normal Enabled", &normal);
			materialData.mat.hasNormal = normal;
		}
		{
			if (ImGui::BeginCombo("SpecularMap", "SpecularMap")) {
				Texture* tex = jshGraphics::GetTextureImGui();
				if (tex != nullptr) SetSpecularMap(tex, matData);
				ImGui::EndCombo();
			}
			bool specular = materialData.mat.hasSpecular;
			ImGui::Checkbox("Specular Enabled", &specular);
			materialData.mat.hasSpecular = specular;
		}

		ImGui::DragFloat("Shininess", &materialData.mat.shininess, 0.03f, 0.01f, 1000.f);
		ImGui::DragFloat("Spec Intensity", &materialData.mat.specIntensity, 0.03f, 0.01f, 1000.f);
		
		materialData.update = true;
	}
#endif

}
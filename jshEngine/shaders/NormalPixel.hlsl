#include "common.hlsli"
#include "Light.hlsli"
#include "Material.hlsli"

Texture2D diffuseMap : register(JSH_GFX_SLOT_TEXTURE2D_DIFFUSE);
Texture2D normalMap : register(JSH_GFX_SLOT_TEXTURE2D_NORMAL);
Texture2D specularMap : register(JSH_GFX_SLOT_TEXTURE2D_SPECULAR);

SamplerState diffuseSampler : register(s0);
SamplerState defSampler : register(s1);

cbuffer mesh : register(b0) {
	bool diffuseEnabled;
	bool normalEnabled;
	bool specularEnabled;
};

float4 main(float3 position : FragPosition, float3x3 tanBiNor : FragTanBiNor,
	float2 texCoord : FragTexCoord, float3 toCamera : FragToCamera) : SV_TARGET
{
	float3 normal;
	if (normalEnabled) {
		normal = normalMap.Sample(defSampler, texCoord).xyz;
		normal.x = (normal.x * 2.f) - 1.f;
		normal.y = (normal.y * 2.f) - 1.f;

		normal = normalize(mul(normal, tanBiNor));
	}
	else {
		normal = normalize(float3(tanBiNor[2][0], tanBiNor[2][1], tanBiNor[2][2]));
	}

	float specI = material.specularIntensity;
	float shiny = material.shininess;
	if (specularEnabled) {
		float4 s = specularMap.Sample(defSampler, texCoord);
		specI *= s.x;
		shiny *= s.w;
	}

	float3 lightColor = LoadLightColor(position, normal, toCamera, specI, shiny);
	
	if (diffuseEnabled) {
		return float4(lightColor, 1.f)* diffuseMap.Sample(diffuseSampler, texCoord);
	}
	else {
		return float4(lightColor, 1.f);
	}
}
#include "Light.hlsli"

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D specularMap : register(t2);

SamplerState diffuseSampler : register(s0);
SamplerState defSampler : register(s1);

cbuffer material : register(b1) {
	bool diffuseEnabled;
	bool normalEnabled;
	bool specularEnabled;
	float shininess;
	float specularIntensity;

	float _internal_Offset0;
	float _internal_Offset1;
	float _internal_Offset2;
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

	float specI = specularIntensity;
	float shiny = shininess;
	if (specularEnabled) {
		float4 s = specularMap.Sample(defSampler, texCoord);
		specI *= s.x;
		shiny *= s.w;
	}

	float3 lightColor = LoadLightColor(position, normalize(normal), toCamera, specI, shiny);

	if (diffuseEnabled) {
		return float4(lightColor, 1.f)* diffuseMap.Sample(diffuseSampler, texCoord);
	}
	else {
		return float4(lightColor, 1.f);
	}
}
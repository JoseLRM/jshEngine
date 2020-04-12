#include "Light.hlsli"

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);

SamplerState diffuseSampler : register(s0);
SamplerState normalSampler : register(s1);

float4 main(float3 position : FragPosition, float3x3 tanBiNor : FragTanBiNor,
	float2 texCoord : FragTexCoord, float3 toCamera : FragToCamera) : SV_TARGET
{
	float3 normal = normalMap.Sample(normalSampler, texCoord).xyz;
	normal.x = (normal.x * 2.f) - 1.f;
	normal.y = (normal.y * 2.f) - 1.f;

	normal = mul(normal, tanBiNor);

	float3 lightColor = LoadLightColor(position, normalize(normal), toCamera, 0.25f, 10.f);
	return float4(lightColor, 1.f) * diffuseMap.Sample(diffuseSampler, texCoord);
}
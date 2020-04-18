#include "Light.hlsli"
#include "Material.hlsli"

float4 main(float3 normal : FragNormal, float3 fragPos : FragPosition, float3 fragColor : FragColor) : SV_TARGET
{
	float3 finalColor = LoadLightColor(fragPos, normal, normalize(fragPos), material.specularIntensity, material.shininess) * fragColor;
	return float4(finalColor, 1.f);
}
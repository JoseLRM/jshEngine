#include "Light.hlsli"
#include "Material.hlsli"

float4 main(float3 normal : FragNormal, float3 fragPos : FragPosition, float3 toCamera : FragToCamera) : SV_TARGET
{
	float3 lightColor = LoadLightColor(fragPos, normal, toCamera, material.specularIntensity, material.shininess);

	return float4(lightColor, 1.f);
}
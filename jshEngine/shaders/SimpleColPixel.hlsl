#include "Light.hlsli"

float4 main(float3 normal : FragNormal, float3 fragPos : FragPosition, float3 fragColor : FragColor) : SV_TARGET
{
	float3 finalColor = LoadLightColor(fragPos, normal, normalize(fragPos), 1.f, 50.f) * fragColor;
	return float4(finalColor, 1.f);
}
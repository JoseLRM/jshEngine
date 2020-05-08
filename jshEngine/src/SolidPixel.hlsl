#include "Light.hlsli"

cbuffer c : register(JSH_GFX_SLOT_CBUFFER_FREE0)
{
	float shininess;
	float specularIntensity;
	float3 color;
};

float4 main(float3 normal : FragNormal, float3 fragPos : FragPosition, float3 toCamera : FragToCamera) : SV_TARGET
{
	float3 lightColor = LoadLightColor(fragPos, normal, toCamera, specularIntensity, shininess);
	return float4(lightColor * color, 1.f);
}
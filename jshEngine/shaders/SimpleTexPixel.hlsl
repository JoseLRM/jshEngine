#include "Light.hlsli"

Texture2D tex;
SamplerState samp;

float4 main(float3 normal : FragNormal, float3 fragPos : FragPosition, float2 texCoord : FragTexCoord, float3 toCamera : FragToCamera) : SV_TARGET
{
	float3 lightColor = LoadLightColor(fragPos, normal, toCamera, 2.f, 20.f);
	return float4(lightColor, 1.f) * tex.Sample(samp, texCoord);
}
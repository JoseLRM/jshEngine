#include "Light.hlsli"

Texture2D tex;
SamplerState samp;

float4 main(float3 normal : FragNormal, float3 fragPos : FragPosition, float2 texCoord : FragTexCoord) : SV_TARGET
{
	float3 lightColor = LoadLightColor(fragPos, normal, normalize(fragPos), 1.f, 50.f);
	return float4(lightColor, 1.f) * tex.Sample(samp, texCoord);
}
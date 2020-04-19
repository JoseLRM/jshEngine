#include "Camera.hlsli"
#include "Instance.hlsli"

struct VS_OUTPUT {
	float3 fragNormal : FragNormal;
	float3 fragPosition : FragPosition;
	float3 fragColor : FragColor;
	float4 position : SV_Position;
};

VS_OUTPUT main(float3 pos : Position, float3 nor : Normal, float3 color : Color)
{
	VS_OUTPUT output;
	output.fragPosition = mul(float4(pos, 1.f), instance.tm);
	output.position = mul(mul(output.fragPosition, camera.vm), camera.pm);
	output.fragNormal = normalize(mul(nor, (float3x3)instance.tm));
	output.fragColor = color;
	return output;
}
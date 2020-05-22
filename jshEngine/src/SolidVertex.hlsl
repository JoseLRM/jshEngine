#include "Camera.hlsli"

struct VS_OUTPUT {
	float3 fragNormal : FragNormal;
	float3 fragPosition : FragPosition;
	float4 position : SV_Position;
};

VS_OUTPUT main(float3 pos : Position, float3 nor : Normal, matrix tm : TM)
{
	VS_OUTPUT output;
	output.fragPosition = mul(float4(pos, 1.f), tm).xyz;
	output.position = mul(float4(output.fragPosition, 1.f), camera.pm);
	output.fragNormal = normalize(mul(nor, (float3x3)tm));

	return output;
}
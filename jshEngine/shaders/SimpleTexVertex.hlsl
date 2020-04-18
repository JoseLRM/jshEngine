#include "common.hlsli"
#include "Camera.hlsli"
#include "Instance.hlsli"

struct VS_OUTPUT {
	float3 fragNormal : FragNormal;
	float3 fragPosition : FragPosition;
	float2 texCoord : FragTexCoord;
	float3 fragToCamera : FragToCamera;
	float4 position : SV_Position;
};

VS_OUTPUT main(float3 pos : Position, float3 nor : Normal, float2 texCoord : TexCoord)
{
	VS_OUTPUT output;
	output.fragPosition = mul(float4(pos, 1.f), instance.tm);
	output.position = mul(mul(float4(output.fragPosition, 1.f), camera.vm), camera.pm);
	output.fragNormal = normalize(mul(nor, (float3x3)instance.tm));
	output.texCoord = texCoord;

	output.fragToCamera = normalize(output.fragPosition + camera.position.xyz);

	return output;
}
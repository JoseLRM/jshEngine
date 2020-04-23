#include "Camera.hlsli"
#include "Instance.hlsli"

struct VS_IN {
	float3 position : Position;
	float3 normal : Normal;
	float2 texCoord : TexCoord;
	float3 tangent : Tangent;
	float3 bitangent : Bitangent;
};

struct VS_OUT {
	float3 fragPos : FragPosition;
	float3x3 tanBiNor : FragTanBiNor;
	float2 texCoord : FragTexCoord;
	float3 toCamera : FragToCamera;
	float4 position : SV_Position;
};

VS_OUT main( VS_IN input )
{
	VS_OUT output;

	output.fragPos = mul(float4(input.position, 1.f), instance.tm);
	output.tanBiNor = mul(float3x3(input.tangent, input.bitangent, input.normal), (float3x3)instance.tm);

	output.texCoord = input.texCoord;

	output.toCamera = normalize(output.fragPos - camera.position.xyz);

	output.position = mul(mul(float4(output.fragPos, 1.f), camera.vm), camera.pm);

	return output;
}
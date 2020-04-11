cbuffer c {
	matrix tm;
	matrix vm;
	matrix pm;
};

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
	output.fragPosition = mul(float4(pos, 1.f), tm);
	output.position = mul(float4(pos ,1.f), mul(mul(tm, vm), pm));
	output.fragNormal = normalize(mul(nor, (float3x3)tm));
	output.texCoord = texCoord;

	float3 cameraPos = float3(vm[3][0], vm[3][1], vm[3][2]);
	output.fragToCamera = normalize(output.fragPosition + cameraPos);

	return output;
}
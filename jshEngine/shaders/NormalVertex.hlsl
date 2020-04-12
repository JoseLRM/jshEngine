cbuffer c {
	matrix tm;
	matrix vm;
	matrix pm;
};

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

	output.fragPos = mul(float4(input.position, 1.f), tm);
	output.tanBiNor = float3x3(normalize(mul(input.tangent, (float3x3)tm)), normalize(mul(input.bitangent, (float3x3)tm)), normalize(mul(input.normal, (float3x3)tm)));

	output.texCoord = input.texCoord;

	float3 cameraPos = float3(vm[3][0], vm[3][1], vm[3][2]);
	output.toCamera = normalize(output.fragPos + cameraPos);

	output.position = mul(mul(float4(output.fragPos, 1.f), vm), pm);

	return output;
}
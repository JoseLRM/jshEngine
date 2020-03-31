cbuffer c {
	matrix tm;
	matrix vm;
	matrix pm;
};

struct VS_OUTPUT {
	float3 fragNormal : FragNormal;
	float3 fragPosition : FragPosition;
	float2 texCoord : FragTexCoord;
	float4 position : SV_Position;
};

VS_OUTPUT main(float3 pos : Position, float3 nor : Normal, float2 texCoord : TexCoord)
{
	VS_OUTPUT output;
	output.fragPosition = mul(float4(pos, 1.f), tm);
	output.position = mul(mul(float4(pos, 1.f), mul(tm, vm)), pm);
	output.fragNormal = normalize(mul(nor, (float3x3)tm));
	output.texCoord = texCoord;
	return output;
}
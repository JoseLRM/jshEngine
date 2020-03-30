cbuffer c {
	matrix tm;
	matrix vm;
	matrix pm;
};

struct VS_OUTPUT {
	float3 fragNormal : FragNormal;
	float3 fragPosition : FragPosition;
	float4 position : SV_Position;
};

VS_OUTPUT main(float3 pos : Position, float3 nor : Normal)
{
	VS_OUTPUT output;
	output.fragPosition = mul(float4(pos, 1.f), tm);
	output.position = mul(mul(float4(pos, 1.f), mul(tm, vm)), pm);
	output.fragNormal = normalize(mul(nor, (float3x3)tm));
	return output;
}
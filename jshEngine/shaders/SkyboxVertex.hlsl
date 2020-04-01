cbuffer c {
	matrix vm;
	matrix pm;
};

struct VS_OUT {
	float2 tc : FragTexCoord;
	float4 position : SV_Position;
};

VS_OUT main( float3 pos : Position, float2 texCoord : TexCoord )
{
	VS_OUT vsOut;
	vsOut.tc = texCoord;
	vsOut.position = mul(float4(mul(pos, (float3x3)vm), 1.f), pm);
	return vsOut;
}
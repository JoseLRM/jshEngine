cbuffer c : register(b0){
	matrix tm;
	matrix vm;
	matrix pm;
};

float4 main(float3 pos : Position) : SV_POSITION
{
	return mul(mul(mul(float4(pos, 1.f), tm), vm), pm);
}
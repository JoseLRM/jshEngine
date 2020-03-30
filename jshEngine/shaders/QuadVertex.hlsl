cbuffer c {
	matrix tm;
	matrix pm;
};

float4 main( float2 pos : Position ) : SV_POSITION
{
	return mul(mul(float4(pos, 0.f, 1.f), tm), pm);
}